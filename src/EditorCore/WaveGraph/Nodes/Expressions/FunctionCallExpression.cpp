#include "WaveGraph/Nodes/Expressions/FunctionCallExpression.h"

namespace Mirage::EditorCore::WaveGraph {

FunctionCallExpression::FunctionCallExpression(WeaveNodeId id, std::shared_ptr<WeaveExpressionNode> obj_node,
                                               const std::string &var_name) : WeaveExpressionNode(id) {
    this->object_node = std::move(obj_node);
    this->func_name = std::move(var_name);
}

WeaveGraphNodeKind FunctionCallExpression::GetKind() const noexcept {
    return WeaveGraphNodeKinds::FunctionCallExpression;
}

WeaveValueKind FunctionCallExpression::GetValueKind() const noexcept {
    return WeaveValueKindValues::Unknown;
}

} // namespace Mirage::EditorCore::WaveGraph
