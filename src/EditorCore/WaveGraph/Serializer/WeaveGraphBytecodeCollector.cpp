#include "WaveGraph/Serializer/WeaveGraphBytecodeCollector.h"

#include "WaveGraph/Nodes/Expressions/BinOps/WeaveBinaryOperationNode.hpp"
#include "WaveGraph/Nodes/Expressions/FunctionCallExpression.h"
#include "WaveGraph/Nodes/Expressions/Variables/GetClassVariableExpression.h"
#include "WaveGraph/Nodes/Expressions/Variables/GetLocalVariableExpression.h"
#include "WaveGraph/Nodes/Statements/ExpressionStatement.h"
#include "WaveGraph/Nodes/Statements/FunctionStatementNode.h"
#include "WaveGraph/Nodes/Statements/SetLocalVariableStatementNode.h"
#include "WaveGraph/WeaveGraph.hpp"
#include <limits>

namespace Mirage::EditorCore::WaveGraph {

void WeaveGraphBytecodeCollector::AppendOpCode(OP_CODE op_code,
                                               FunctionProcessingContext &context) {
    context.bytecode_info.bytecode.push_back(static_cast<uint8_t>(op_code));
}

WeaveGraphBytecodeCollector &WeaveGraphBytecodeCollector::GetInstance() {
    static WeaveGraphBytecodeCollector  instance;
    return instance;
}

void WeaveGraphBytecodeCollector::CollectGraph(std::shared_ptr<WeaveGraph> graph, GraphBytecode* target_info) {
    for (WeaveNodeId func_node_id : graph->function_statements) {
        const auto &func_stmt_node = graph->GetNode(func_node_id);
        if (func_stmt_node == nullptr) {
            continue;
        }

        ProcessFunctionStatementNode(
            std::static_pointer_cast<FunctionStatementNode>(func_stmt_node), target_info);
    }
}

void WeaveGraphBytecodeCollector::ProcessFunctionStatementNode(
    std::shared_ptr<FunctionStatementNode> function_stmt_node, GraphBytecode *target_info) {

    MFunctionSignature function_signature = function_stmt_node->GetSignature();


    FunctionBytecodeInfo bytecode_info = {
        .strings = {},
             .bytecode = {},
        .nameRelocs = {}
    };

    FunctionProcessingContext processing_context = {
        .bytecode_info = bytecode_info,
        .string_offsets = {},
        .local_vars = function_stmt_node->GetGlobToLocalIds(),
    };

    const auto& function_local_vars = function_stmt_node->GetLocalVariables();
    for (size_t i = function_signature.params.size() + 1; i < function_local_vars.size(); i++) {
        if (i < 255) {
            AppendOpCode(OP_CODE::INIT_LOCAL_VAR, processing_context);
            AppendNBytes<uint8_t>(i, processing_context);
            AppendMType(function_local_vars[i].signature.type, processing_context);
        } else {
            AppendOpCode(OP_CODE::INIT_LOCAL_VAR_EXT, processing_context);
            AppendNBytes<uint32_t>(i, processing_context);
            AppendMType(function_local_vars[i].signature.type, processing_context);
        }
    }


    ProcessStatementNode(function_stmt_node->GetNextStatement(), processing_context);
    AppendOpCode(OP_CODE::RET, processing_context);

    target_info->function_metas.push_back({
        .name = function_stmt_node->GetName(),
        .signature = function_signature,
        .bytecode_info = std::move(bytecode_info)
    });
}

void WeaveGraphBytecodeCollector::AppendMType(const MType &type,
                                              FunctionProcessingContext &processing_context) {

    auto& bytecode = processing_context.bytecode_info.bytecode;

    bytecode.push_back(static_cast<char>(type.type));

    if (type.type_args.size() > std::numeric_limits<std::uint8_t>::max()) {
        throw std::runtime_error("Too many MType type arguments");
    }

    bytecode.push_back(static_cast<char>(type.type_args.size()));

    for (const MType& type_arg : type.type_args) {
        AppendMType(type_arg, processing_context);
    }
}

void WeaveGraphBytecodeCollector::ProcessStatementNode(
    std::shared_ptr<StatementNode> stmt_node, FunctionProcessingContext &processing_context) {
    if (stmt_node == nullptr)
        return;

    switch (stmt_node->GetKind()) {
    case WeaveGraphNodeKinds::SetLocalStatement: {
        auto setLocalStatement = std::static_pointer_cast<SetLocalVariableStatementNode>(stmt_node);
        if (setLocalStatement->value_ == nullptr)
            break;

        ProcessExpression(setLocalStatement->value_, processing_context);
        const auto localVarSlot = processing_context.local_vars.at(setLocalStatement->varId_);
        if (localVarSlot < 255) {
            AppendOpCode(OP_CODE::STORE_LOCAL_VAR, processing_context);
            AppendNBytes<uint8_t>(static_cast<uint8_t>(localVarSlot), processing_context);
        } else {
            AppendOpCode(OP_CODE::STORE_LOCAL_VAR_EXT, processing_context);
            AppendNBytes<uint32_t>(localVarSlot, processing_context);
        }
        break;
    }
    case WeaveGraphNodeKinds::ExpressionStatement: {
        auto expressionNode = std::static_pointer_cast<ExpressionStatement>(stmt_node);
        ProcessExpression(expressionNode->GetExpressionNode(), processing_context);
        break;
    }
    default:
        throw std::runtime_error("ERROR LOADING STATEMENT NODE, INVALID TYPE");
    }
    ProcessStatementNode(stmt_node->GetNextStatement(), processing_context);
}

void WeaveGraphBytecodeCollector::ProcessExpression(std::shared_ptr<WeaveExpressionNode> expr_node,
                                                    FunctionProcessingContext &processing_context) {
    if (expr_node->GetKind() == WeaveGraphNodeKinds::Constant) {
        auto const_node = std::static_pointer_cast<WeaveConstantNode>(expr_node);
        std::visit(
            [this, &processing_context](const auto &value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    AppendOpCode(OP_CODE::PUSH_STR, processing_context);
                    uint32_t rip = processing_context.GetStringOffset(value) + processing_context.bytecode_info.bytecode.size() + 4;
                    AppendNBytes<int32_t>(-static_cast<int32_t>(rip), processing_context);
                } else if constexpr (std::is_same_v<T, std::int64_t>) {
                    AppendOpCode(OP_CODE::PUSH_CONST, processing_context);
                    AppendNBytes<uint8_t>(MValueTypes::INT64, processing_context);
                    AppendNBytes<int64_t>(value, processing_context);
                } else if constexpr (std::is_same_v<T, double>) {
                    AppendOpCode(OP_CODE::PUSH_CONST, processing_context);
                    AppendNBytes<uint8_t>(MValueTypes::DOUBLE, processing_context);
                    AppendNBytes<double>(value, processing_context);
                } else if constexpr (std::is_same_v<T, float>) {
                    AppendOpCode(OP_CODE::PUSH_CONST, processing_context);
                    AppendNBytes<uint8_t>(MValueTypes::FLOAT, processing_context);
                    AppendNBytes<float>(value, processing_context);
                } else if constexpr (std::is_same_v<T, bool>) {
                    AppendOpCode(OP_CODE::PUSH_CONST, processing_context);
                    AppendNBytes<uint8_t>(MValueTypes::BOOL, processing_context);
                    AppendNBytes<int>(value, processing_context);
                }
                return WeaveValueKindValues::Unknown;
            },
            const_node->value_);

    } else if (WeaveGraphNodeKinds::IsBinaryOperationKind(expr_node->GetKind())) {
        std::static_pointer_cast<WeaveBinaryOperationNode>(expr_node);
        ProcessBinaryExpression(std::static_pointer_cast<WeaveBinaryOperationNode>(expr_node),
                                processing_context);
    } else {
        switch (expr_node->GetKind()) {
        case WeaveGraphNodeKinds::GetClassVariableExpression: {
            auto classVarExpr = std::static_pointer_cast<GetClassVariableExpression>(expr_node);
            if (classVarExpr->object_node == nullptr) {
                throw std::runtime_error("EXPRESSION NOT HAVE INPUT");
            }
            ProcessExpression(classVarExpr->object_node, processing_context);
            AppendOpCode(OP_CODE::GET_OBJECT_VARIABLE, processing_context);
            processing_context.AddNameReloc(processing_context.bytecode_info.bytecode.size(), classVarExpr->var_name);
            AppendNBytes<MNameID>(0, processing_context);
            break;
        }
        case WeaveGraphNodeKinds::GetLocalVariableExpression: {
            auto getLocalVarExpr = std::static_pointer_cast<GetLocalVariableExpression>(expr_node);
            const auto localVarSlot = processing_context.local_vars.at(getLocalVarExpr->var_id);
            if (localVarSlot < 255) {
                AppendOpCode(OP_CODE::LOAD_LOCAL_VAR, processing_context);
                AppendNBytes<uint8_t>(static_cast<uint8_t>(localVarSlot), processing_context);
            } else {
                AppendOpCode(OP_CODE::LOAD_LOCAL_VAR_EXT, processing_context);
                AppendNBytes<uint32_t>(localVarSlot, processing_context);
            }
            break;
        }
        case WeaveGraphNodeKinds::FunctionCallExpression: {
            auto classVarExpr = std::static_pointer_cast<FunctionCallExpression>(expr_node);
            if (classVarExpr->object_node == nullptr) {
                throw std::runtime_error("EXPRESSION NOT HAVE INPUT");
            }
            ProcessExpression(classVarExpr->object_node, processing_context);
            AppendOpCode(OP_CODE::GET_FUNCTION, processing_context);
            processing_context.AddNameReloc(processing_context.bytecode_info.bytecode.size(), classVarExpr->func_name);
            AppendNBytes<MNameID>(0, processing_context);

            for (const auto &inputExpr : classVarExpr->inputs) {
                if (inputExpr == nullptr) {
                    throw std::runtime_error("EXPRESSION NOT HAVE INPUT");
                }
                ProcessExpression(inputExpr, processing_context);
            }

            AppendOpCode(OP_CODE::CALL, processing_context);
            break;
        }
        default:
            throw std::runtime_error("ERROR LOADING EXPR NODE, INVALID TYPE");
        }
    }
}
void WeaveGraphBytecodeCollector::ProcessBinaryExpression(
    std::shared_ptr<WeaveBinaryOperationNode> binOpExpr,
    FunctionProcessingContext &processing_context) {
    if (binOpExpr->GetLeftOperand() == nullptr || binOpExpr->GetRightOperand() == nullptr) {
        throw std::runtime_error("ERROR binary expression , one of operands is nullptr");
    }
    OP_CODE op_code;
    switch (binOpExpr->GetKind()) {
    case WeaveGraphNodeKinds::SumOperation: {
        op_code = OP_CODE::SUM;
        break;
    }
    case WeaveGraphNodeKinds::SubOperation: {
        op_code = OP_CODE::SUB;
        break;
    }
    case WeaveGraphNodeKinds::MulOperation: {
        op_code = OP_CODE::MUL;
        break;
    }
    case WeaveGraphNodeKinds::DivideOperation: {
        op_code = OP_CODE::DIV;
        break;
    }
    default:
        throw std::runtime_error("ERROR binary expression , one of operands is nullptr");
    }

    ProcessExpression(binOpExpr->GetLeftOperand(), processing_context);
    ProcessExpression(binOpExpr->GetRightOperand(), processing_context);
    AppendOpCode(op_code, processing_context);
}

} // namespace Mirage::EditorCore::WaveGraph