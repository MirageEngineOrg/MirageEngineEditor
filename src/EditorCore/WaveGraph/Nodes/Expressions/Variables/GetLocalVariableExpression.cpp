#include "WaveGraph/Nodes/Expressions/Variables/GetLocalVariableExpression.h"

namespace Mirage::EditorCore::WaveGraph {

GetLocalVariableExpression::GetLocalVariableExpression(WeaveNodeId id, LocalVariableId var_id) : WeaveExpressionNode(id) {
    this->var_id = var_id;
}
WeaveGraphNodeKind GetLocalVariableExpression::GetKind() const noexcept {
    return WeaveGraphNodeKinds::GetLocalVariableExpression;
}
WeaveValueKind GetLocalVariableExpression::GetValueKind() const noexcept {
    return WeaveValueKindValues::Unknown;
}
LocalVariableId GetLocalVariableExpression::GetVariableId() const noexcept {
    return this->var_id;
}

} // namespace Mirage::EditorCore::WaveGraph
