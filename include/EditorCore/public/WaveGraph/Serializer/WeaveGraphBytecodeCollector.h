#pragma once
#include "WaveGraph/Nodes/Base/LocalVariable.h"

#include <Bytecode.h>
#include <Meta/Field/MFunction.h>
#include <Name/MName.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct FunctionBytecodeInfo {
    std::vector<std::string> strings;
    std::vector<char> bytecode;
    std::unordered_map<uint32_t, uint32_t> nameRelocs; // bytecode_offset -> offset from strings
};

struct FunctionMetaInfo {
    std::string name;
    MFunctionSignature signature;
    FunctionBytecodeInfo bytecode_info;
};

struct GraphBytecode{
    std::vector<FunctionMetaInfo> function_metas;
};

struct FunctionProcessingContext {
    FunctionBytecodeInfo& bytecode_info;
    std::unordered_map<std::string, uint32_t> string_offsets;
    const std::unordered_map<LocalVariableId, uint32_t> &local_vars;

    uint32_t GetStringOffset(const std::string& str) {
        if (!string_offsets.contains(str)) {
            uint32_t prev_offset = 0;

            if (!bytecode_info.strings.empty()) {
                prev_offset = string_offsets[bytecode_info.strings.back()];
            }
            bytecode_info.strings.push_back(str);
            string_offsets[str] = prev_offset + str.size() + 1;
        }
        return string_offsets[str];
    }

    void AddNameReloc(uint32_t offset, const std::string& name) {
        uint32_t string_offset = GetStringOffset(name);
        bytecode_info.nameRelocs[offset] = string_offset;
    }
};

namespace Mirage::EditorCore::WaveGraph {
class WeaveBinaryOperationNode;
class WeaveExpressionNode;
class StatementNode;
class FunctionStatementNode;
class WeaveGraph;

class WeaveGraphBytecodeCollector {
public:
    static WeaveGraphBytecodeCollector& GetInstance();
    void CollectGraph(std::shared_ptr<WeaveGraph>, GraphBytecode* target_info);

    void ProcessFunctionStatementNode(std::shared_ptr<FunctionStatementNode> function_stmt_node, GraphBytecode *target_info);
private:
    WeaveGraphBytecodeCollector() = default;
    void AppendOpCode(OP_CODE op_code, FunctionProcessingContext &context);

    template<typename T>
    void AppendNBytes(T value, FunctionProcessingContext &processing_context);

    void AppendMType(const MType& type, FunctionProcessingContext &processing_context);

    void ProcessStatementNode(std::shared_ptr<StatementNode> stmt_node, FunctionProcessingContext & processing_context);
    void ProcessExpression(std::shared_ptr<WeaveExpressionNode> stmt_node,
                           FunctionProcessingContext &processing_context);
    void ProcessBinaryExpression(std::shared_ptr<WeaveBinaryOperationNode> stmt_node,
                                 FunctionProcessingContext &processing_context);

};
template <typename T> void WeaveGraphBytecodeCollector::AppendNBytes(T value,
                                               FunctionProcessingContext &processing_context) {
    auto* curr_ptr = (unsigned char*)&value;

    for (int i = 0; i < sizeof(T); i++) {
        processing_context.bytecode_info.bytecode.push_back(*curr_ptr);
        ++curr_ptr;
    }
}

}