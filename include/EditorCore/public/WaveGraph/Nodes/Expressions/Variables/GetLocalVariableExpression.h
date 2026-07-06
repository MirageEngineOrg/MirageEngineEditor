#pragma once
#include "WaveGraph/Nodes/Base/LocalVariable.h"
#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"

namespace Mirage::EditorCore::WaveGraph {

class GetLocalVariableExpression : public WeaveExpressionNode {
public:
    explicit GetLocalVariableExpression(WeaveNodeId id, LocalVariableId var_id);
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;

    [[nodiscard]] LocalVariableId GetVariableId() const noexcept;
    LocalVariableId var_id;
};


}
