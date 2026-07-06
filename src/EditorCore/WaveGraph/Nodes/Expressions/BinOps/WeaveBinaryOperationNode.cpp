#include "WaveGraph/Nodes/Expressions/BinOps/WeaveBinaryOperationNode.hpp"
#include <utility>

namespace Mirage::EditorCore::WaveGraph {

WeaveBinaryOperationNode::WeaveBinaryOperationNode(
    const WeaveNodeId id,
    std::shared_ptr<WeaveExpressionNode> leftOperand,
    std::shared_ptr<WeaveExpressionNode> rightOperand
)
    : WeaveExpressionNode(id)
    , leftOperand_(std::move(leftOperand))
    , rightOperand_(std::move(rightOperand)) {
}

WeaveGraphNodeKind WeaveBinaryOperationNode::GetKind() const noexcept {
    return WeaveGraphNodeKinds::BinaryOperation;
}

const std::shared_ptr<WeaveExpressionNode>& WeaveBinaryOperationNode::GetLeftOperand() const noexcept {
    return leftOperand_;
}

const std::shared_ptr<WeaveExpressionNode>& WeaveBinaryOperationNode::GetRightOperand() const noexcept {
    return rightOperand_;
}
void WeaveBinaryOperationNode::SetLeftOperand(
    std::shared_ptr<WeaveExpressionNode> leftOperand) noexcept {
    this->leftOperand_ = std::move(leftOperand);
}
void WeaveBinaryOperationNode::SetRightOperand(
    std::shared_ptr<WeaveExpressionNode> rightOperand) noexcept {
    this->rightOperand_ = std::move(rightOperand);
}

} // namespace Mirage::EditorCore::WaveGraph
