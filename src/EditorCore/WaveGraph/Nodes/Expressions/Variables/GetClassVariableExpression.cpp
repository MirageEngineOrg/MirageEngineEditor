#include "WaveGraph/Nodes/Expressions/Variables/GetClassVariableExpression.h"

namespace Mirage::EditorCore::WaveGraph {

GetClassVariableExpression::GetClassVariableExpression(WeaveNodeId id,
                                                       std::shared_ptr<WeaveExpressionNode> object_node,
                                                       const std::string &var_name) : WeaveExpressionNode(id) {

    this->object_node = std::move(object_node);
    this->var_name = var_name;
}
WeaveGraphNodeKind GetClassVariableExpression::GetKind() const noexcept {
    return WeaveGraphNodeKinds::GetClassVariableExpression ;
}
WeaveValueKind GetClassVariableExpression::GetValueKind() const noexcept {
    return WeaveValueKindValues::Unknown;

}

const std::shared_ptr<WeaveExpressionNode> & GetClassVariableExpression::GetSourceObjectNode() const noexcept {
    return this->object_node;
}

} // namespace Mirage::EditorCore::WaveGraph
