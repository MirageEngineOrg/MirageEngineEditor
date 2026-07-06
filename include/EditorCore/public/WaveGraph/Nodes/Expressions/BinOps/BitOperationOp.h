#pragma once
#include "WeaveBinaryOperationNode.hpp"

namespace Mirage::EditorCore::WaveGraph {

class BitOperationOp : public WeaveBinaryOperationNode{
public:
    BitOperationOp(WeaveGraphNodeKind actual_kind, WeaveNodeId id, const std::shared_ptr<WeaveExpressionNode> &leftOperand = nullptr,
                       const std::shared_ptr<WeaveExpressionNode> &rightOperand = nullptr);

    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;
private:
    WeaveGraphNodeKind actual_kind;
};

class BitOpInputConnectionHandler : public BinOpInputConnectionHandler<BitOperationOp> {
public:
    BitOpInputConnectionHandler(const std::weak_ptr<BitOperationOp> &node, bool bIsLeft)
        : BinOpInputConnectionHandler(node, bIsLeft) {}

    bool ValidateConnection(std::shared_ptr<WeaveGraphNode> targetNode) override;
};

}