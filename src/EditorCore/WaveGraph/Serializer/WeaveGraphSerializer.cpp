#pragma once
#include <utility>

#include "WaveGraph/Nodes/Expressions/FunctionCallExpression.h"
#include "WaveGraph/Nodes/Expressions/Variables/GetLocalVariableExpression.h"
#include "WaveGraph/Nodes/Statements/ExpressionStatement.h"
#include "WaveGraph/Nodes/Statements/FunctionStatementNode.h"
#include "WaveGraph/Serializer/WeaveGraphSerializer.h"
#include "WaveGraph/WeaveGraph.hpp"

#include <algorithm>

namespace Mirage::EditorCore::WaveGraph {

WeaveGraphSerializer &WeaveGraphSerializer::GetInstance() {
    static WeaveGraphSerializer instance;
    return instance;
}

void WeaveGraphSerializer::Serialize(const fs::path &path, std::shared_ptr<WeaveGraph> graph) {
    ByteCodeFileWriter writer(path);

    GraphBytecode init_func_collector_info;
    std::unordered_map<uint32_t, uint32_t> relocs_map;
    SerializeFunctions(&init_func_collector_info, relocs_map, std::move(graph), writer);

    SerializeFunctionMeta(init_func_collector_info.function_metas[0], writer);
    uint32_t init_offset = writer.GetCurrentFileOffset() - init_func_collector_info.function_metas[0].bytecode_info.bytecode.size();

    uint32_t relocs_offset = writer.GetCurrentFileOffset();
    writer.WriteRelocs(relocs_map);
    writer.WriteHeader(relocs_offset, init_offset);
}
void WeaveGraphSerializer::SerializeFunctions(GraphBytecode *init_function_bytecode,
                                              std::unordered_map<uint32_t, uint32_t> &relocs,
                                              std::shared_ptr<WeaveGraph> graph,
                                              ByteCodeFileWriter &writer) {
    GraphBytecode collector_info;
    WeaveGraphBytecodeCollector &collector = WeaveGraphBytecodeCollector::GetInstance();
    collector.CollectGraph(std::move(graph), &collector_info);

    std::shared_ptr<WeaveGraph> temp_graph = std::make_shared<WeaveGraph>();

    std::shared_ptr<FunctionStatementNode> initialize_function =
        temp_graph->CreateNode<FunctionStatementNode>("__Initialize", std::vector<LocalVariable>{},
                                                      MType{.type = MValueTypes::VOID},
                                                      std::vector<LocalVariable>{});

    auto thisNode = temp_graph->CreateNode<GetLocalVariableExpression>(0);
    std::shared_ptr<StatementNode> currStatementNode = initialize_function;

    for (FunctionMetaInfo &function_meta : collector_info.function_metas) {
        uint32_t current_file_offset = writer.GetCurrentFileOffset();
        uint32_t targetBytecodeOffset = current_file_offset + sizeof(uint32_t);
        for (auto it : function_meta.bytecode_info.strings) {
            targetBytecodeOffset += it.size() + 1;
        }

        for (auto it : function_meta.bytecode_info.nameRelocs) {
            relocs[targetBytecodeOffset + it.first] = targetBytecodeOffset - it.second;
        }

        auto funcNameConstant = temp_graph->CreateConstantNode(function_meta.name);
        auto register_call_func =
            temp_graph->CreateNode<FunctionCallExpression>(thisNode, "RegisterVMFunction");
        register_call_func->inputs.push_back(funcNameConstant);

        auto functionCallExpressionStatement =
            temp_graph->CreateNode<ExpressionStatement>(register_call_func);
        currStatementNode->SetNextStatement(functionCallExpressionStatement);
        currStatementNode = functionCallExpressionStatement;


        SerializeFunctionMeta(function_meta, writer);
    }

    collector.ProcessFunctionStatementNode(initialize_function, init_function_bytecode);
    auto &function_meta = init_function_bytecode->function_metas[0];
    uint32_t current_file_offset = writer.GetCurrentFileOffset();
    uint32_t targetBytecodeOffset = current_file_offset + sizeof(uint32_t);
    for (auto it : function_meta.bytecode_info.strings) {
        targetBytecodeOffset += it.size() + 1;
    }

    for (auto it : function_meta.bytecode_info.nameRelocs) {
        relocs[targetBytecodeOffset + it.first] = targetBytecodeOffset - it.second;
    }
}

void WeaveGraphSerializer::SerializeFunctionMeta(const FunctionMetaInfo &function_meta,
                                                 ByteCodeFileWriter &writer) {
    auto reversed_strtab = function_meta.bytecode_info.strings;
    std::reverse(reversed_strtab.begin(), reversed_strtab.end());
    writer.SerializeStringTable(reversed_strtab);
    writer.SerializeBytearray(function_meta.bytecode_info.bytecode);
}

} // namespace Mirage::EditorCore::WaveGraph
