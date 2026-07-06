#pragma once
#include "StatementNode.h"
#include "WaveGraph/Nodes/Base/LocalVariable.h"
#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"

namespace Mirage::EditorCore::WaveGraph {

class SetLocalVariableStatementNode : public StatementNode {
public:
    SetLocalVariableStatementNode(WeaveNodeId nodeId, LocalVariableId var_id, const std::shared_ptr<WeaveExpressionNode> &value) : StatementNode(nodeId), varId_(var_id), value_(value) {}
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;

    LocalVariableId varId_;
    std::shared_ptr<WeaveExpressionNode> value_;
};

}
