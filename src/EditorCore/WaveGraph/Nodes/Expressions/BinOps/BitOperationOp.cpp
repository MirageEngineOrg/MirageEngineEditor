#include "WaveGraph/Nodes/Expressions/BinOps/BitOperationOp.h"

namespace Mirage::EditorCore::WaveGraph {

BitOperationOp::BitOperationOp(WeaveGraphNodeKind actual_kind, WeaveNodeId id,
                               const std::shared_ptr<WeaveExpressionNode> &leftOperand,
                               const std::shared_ptr<WeaveExpressionNode> &rightOperand) : WeaveBinaryOperationNode(id, leftOperand, rightOperand) {
    this->actual_kind = actual_kind;
}

WeaveGraphNodeKind BitOperationOp::GetKind() const noexcept {
    return actual_kind;
}

WeaveValueKind BitOperationOp::GetValueKind() const noexcept {
    const auto& left = GetLeftOperand();
    const auto& right = GetRightOperand();

    if (left == nullptr || right == nullptr) {
        return  WeaveValueKindValues::Unknown;
    }

    if (left->GetValueKind() != WeaveValueKindValues::Integer ||
        right->GetValueKind() != WeaveValueKindValues::Integer) return WeaveValueKindValues::Unknown;

    return WeaveValueKindValues::Integer;
}
bool BitOpInputConnectionHandler::ValidateConnection(std::shared_ptr<WeaveGraphNode> targetNode) {
    if (targetNode == nullptr) return false;
    if (targetNode->GetKind() == WeaveValueKindValues::Unknown) return true; //for now it will process the connection with invalid, which is actually is unknow , need to be renamed
    if (!WeaveGraphNodeKinds::IsExpressionKind(targetNode->GetKind())) return false;

    auto expression_target = std::static_pointer_cast<WeaveExpressionNode>(targetNode);
    WeaveValueKind targetOperandValueKind = expression_target->GetValueKind();

    return targetOperandValueKind == WeaveValueKindValues::Integer || targetOperandValueKind == WeaveValueKindValues::Unknown;
}

} // namespace Mirage::EditorCore::WaveGraph
