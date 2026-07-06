#pragma once
#include "WaveGraph/Nodes/Base/LocalVariable.h"
#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"

#include <memory>

namespace Mirage::EditorCore::WaveGraph {

class GetClassVariableExpression : public WeaveExpressionNode {
public:
    explicit GetClassVariableExpression(WeaveNodeId id, std::shared_ptr<WeaveExpressionNode>, const std::string& var_name);
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;

    [[nodiscard]] const std::shared_ptr<WeaveExpressionNode> & GetSourceObjectNode() const noexcept;
    std::string var_name;
    std::shared_ptr<WeaveExpressionNode> object_node;
};


}
