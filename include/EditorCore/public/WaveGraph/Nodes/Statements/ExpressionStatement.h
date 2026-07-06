#pragma once
#include "StatementNode.h"

namespace  Mirage::EditorCore::WaveGraph {
class WeaveExpressionNode;

class ExpressionStatement : public StatementNode {
public:
    ExpressionStatement(WeaveNodeId nodeId, std::shared_ptr<WeaveExpressionNode> expression_node);
    [[nodiscard]] std::shared_ptr<WeaveExpressionNode> GetExpressionNode() const;
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
private:
    std::shared_ptr<WeaveExpressionNode> expression_node;
};

}