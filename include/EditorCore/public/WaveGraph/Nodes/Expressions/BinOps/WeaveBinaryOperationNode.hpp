#pragma once

#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"
#include "WaveGraph/WeaveNodeRegistryTypes.hpp"

#include <memory>

namespace Mirage::EditorCore::WaveGraph {

class WeaveBinaryOperationNode : public WeaveExpressionNode {
public:
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] const std::shared_ptr<WeaveExpressionNode>& GetLeftOperand() const noexcept;
    [[nodiscard]] const std::shared_ptr<WeaveExpressionNode>& GetRightOperand() const noexcept;
    void SetLeftOperand(std::shared_ptr<WeaveExpressionNode> leftOperand) noexcept;
    void SetRightOperand(std::shared_ptr<WeaveExpressionNode> rightOperand) noexcept;

    bool ValidateLeftOperand() const { return false; }
protected:
    WeaveBinaryOperationNode(WeaveNodeId id,
        std::shared_ptr<WeaveExpressionNode> leftOperand,
        std::shared_ptr<WeaveExpressionNode> rightOperand
    );

private:
    std::shared_ptr<WeaveExpressionNode> leftOperand_;
    std::shared_ptr<WeaveExpressionNode> rightOperand_;
};


template<typename T>
class BinOpInputConnectionHandler : public InputConnectionHandler {
public:
    BinOpInputConnectionHandler(std::weak_ptr<T> node, bool bIsLeft) : node(std::move(node)), bIsLeft(bIsLeft) {
        static_assert(std::is_base_of_v<WeaveBinaryOperationNode, T>);
    }
    bool Connect(std::shared_ptr<WeaveGraphNode> targetNode) override;

    bool bIsLeft;
    std::weak_ptr<T> node;
};

template <typename T>
bool BinOpInputConnectionHandler<T>::Connect(std::shared_ptr<WeaveGraphNode> targetNode) {
    if (targetNode == nullptr) return false;
    if (WeaveGraphNodeKinds::IsExpressionKind(targetNode->GetKind())) {
        return false;
    }
    if (auto ptr = node.lock()) {
        auto final_target = std::static_pointer_cast<WeaveExpressionNode>(targetNode);
        if (bIsLeft) {
            ptr->SetLeftOperand(final_target);
        } else {
            ptr->SetRightOperand(final_target);
        }
        return true;
    } else return false;
}

} // namespace Mirage::EditorCore::WaveGraph
