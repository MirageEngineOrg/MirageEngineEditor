#pragma once
#include "WaveGraph/WeaveGraphNode.hpp"
#include <memory>

namespace  Mirage::EditorCore::WaveGraph {

class StatementNode : public WeaveGraphNode {
public:
    StatementNode(WeaveNodeId nodeId) : WeaveGraphNode(nodeId) {}

    [[nodiscard]] const std::shared_ptr<StatementNode>& GetNextStatement() const noexcept;
    [[nodiscard]] const std::shared_ptr<StatementNode>& GetPrevStatement() const noexcept;
    void SetNextStatement(std::shared_ptr<StatementNode> nextStatement) noexcept;
    void SetPrevStatement(std::shared_ptr<StatementNode> prevStatement) noexcept;


private:
    std::shared_ptr<StatementNode> nextStatement;
    std::shared_ptr<StatementNode> prevStatement;
};

}