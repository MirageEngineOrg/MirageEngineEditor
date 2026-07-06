#pragma once
#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"
#include "WeaveBinaryOperationNode.hpp"

namespace Mirage::EditorCore::WaveGraph {

class WeaveBaseNumericOp : public WeaveBinaryOperationNode {
public:
    WeaveBaseNumericOp(WeaveGraphNodeKind actual_kind, WeaveNodeId id, const std::shared_ptr<WeaveExpressionNode> &leftOperand = nullptr,
                       const std::shared_ptr<WeaveExpressionNode> &rightOperand = nullptr);

    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;
private:
    WeaveGraphNodeKind actual_kind;
};

class BaseNumericOpInputConnectionHandler : public BinOpInputConnectionHandler<WeaveBaseNumericOp> {
public:
    BaseNumericOpInputConnectionHandler(const std::weak_ptr<WeaveBaseNumericOp> &node, bool bIsLeft)
        : BinOpInputConnectionHandler<WeaveBaseNumericOp>(node, bIsLeft) {}

    bool ValidateConnection(std::shared_ptr<WeaveGraphNode> targetNode) override;
};

}

