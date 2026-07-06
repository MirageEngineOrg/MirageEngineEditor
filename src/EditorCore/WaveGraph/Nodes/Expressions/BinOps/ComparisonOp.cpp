#include "WaveGraph/Nodes/Expressions/BinOps/ComparisonOp.h"

#include "WaveGraph/NodeKindHelper.h"

namespace Mirage::EditorCore::WaveGraph {

ComparisonOp::ComparisonOp(WeaveGraphNodeKind actual_kind, WeaveNodeId id,
                           const std::shared_ptr<WeaveExpressionNode> &leftOperand,
                           const std::shared_ptr<WeaveExpressionNode> &rightOperand) : WeaveBinaryOperationNode(id, leftOperand, rightOperand) {
    this->actual_kind = actual_kind;
}

WeaveGraphNodeKind ComparisonOp::GetKind() const noexcept {
    return actual_kind;
}

WeaveValueKind ComparisonOp::GetValueKind() const noexcept {
    const auto& left = GetLeftOperand();
    const auto& right = GetRightOperand();

    if (left == nullptr || right == nullptr) {
        return  WeaveValueKindValues::Unknown;
    }

    WeaveValueKind left_kind = left->GetValueKind();
    WeaveValueKind right_kind = left->GetValueKind();

    if (!(NodeKindHelper::is_kind_comparable(left_kind) && NodeKindHelper::is_kind_comparable(right_kind))) return WeaveValueKindValues::Unknown;

    return WeaveValueKindValues::Bool;
}
bool ComparisonOpInputConnectionHandler::ValidateConnection(
    std::shared_ptr<WeaveGraphNode> targetNode) {

    if (targetNode == nullptr) return false;
    if (targetNode->GetKind() == WeaveValueKindValues::Unknown) return true; //for now it will process the connection with invalid, which is actually is unknow , need to be renamed
    if (!WeaveGraphNodeKinds::IsExpressionKind(targetNode->GetKind())) return false;
    auto expression_target = std::static_pointer_cast<WeaveExpressionNode>(targetNode);
    WeaveValueKind targetOperandValueKind = expression_target->GetValueKind();
    if (targetOperandValueKind == WeaveValueKindValues::Unknown) return true;
    if (!NodeKindHelper::is_kind_comparable(targetOperandValueKind)) return false;
    return true;
}

} // namespace Mirage::EditorCore::WaveGraph
