#include "WaveGraph/Nodes/Statements/StatementNode.h"


namespace  Mirage::EditorCore::WaveGraph {

const std::shared_ptr<StatementNode> &StatementNode::GetNextStatement() const noexcept {
    return nextStatement;
}
const std::shared_ptr<StatementNode> &StatementNode::GetPrevStatement() const noexcept {
    return prevStatement;
}
void StatementNode::SetNextStatement(std::shared_ptr<StatementNode> nextStatement) noexcept {
    this->nextStatement = std::move(nextStatement);
}
void StatementNode::SetPrevStatement(std::shared_ptr<StatementNode> prevStatement) noexcept {
    this->prevStatement = std::move(prevStatement);
}
} // namespace Mirage::EditorCore::WaveGraph
