#pragma once
#include "WeaveBinaryOperationNode.hpp"

namespace Mirage::EditorCore::WaveGraph {

class ComparisonOp : public WeaveBinaryOperationNode{
public:
    ComparisonOp(WeaveGraphNodeKind actual_kind, WeaveNodeId id, const std::shared_ptr<WeaveExpressionNode> &leftOperand = nullptr,
                       const std::shared_ptr<WeaveExpressionNode> &rightOperand = nullptr);

    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;
private:
    WeaveGraphNodeKind actual_kind;
};

class ComparisonOpInputConnectionHandler : public BinOpInputConnectionHandler<ComparisonOp> {
public:
    ComparisonOpInputConnectionHandler(const std::weak_ptr<ComparisonOp> &node, bool bIsLeft)
        : BinOpInputConnectionHandler(node, bIsLeft) {}

    bool ValidateConnection(std::shared_ptr<WeaveGraphNode> targetNode) override;
};

}