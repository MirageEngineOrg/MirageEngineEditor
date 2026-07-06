#include "WaveGraph/Nodes/Statements/ExpressionStatement.h"

#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"

namespace  Mirage::EditorCore::WaveGraph {

ExpressionStatement::ExpressionStatement(WeaveNodeId nodeId, std::shared_ptr<WeaveExpressionNode> expression_node)
    : StatementNode(nodeId), expression_node(std::move(expression_node)) {

}
std::shared_ptr<WeaveExpressionNode> ExpressionStatement::GetExpressionNode() const {
    return this->expression_node;
}
WeaveGraphNodeKind ExpressionStatement::GetKind() const noexcept {
    return WeaveGraphNodeKinds::ExpressionStatement;
}

} // namespace Mirage::EditorCore::WaveGraph