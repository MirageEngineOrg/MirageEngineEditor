#pragma once
#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"

#include <memory>
#include <vector>

namespace Mirage::EditorCore::WaveGraph {
class FunctionCallExpression : public WeaveExpressionNode {
public:
    explicit FunctionCallExpression(WeaveNodeId id, std::shared_ptr<WeaveExpressionNode> obj_node, const std::string& var_name);
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;

    [[nodiscard]] const std::shared_ptr<WeaveExpressionNode> & GetSourceObjectNode() const noexcept;
    std::string func_name;
    std::shared_ptr<WeaveExpressionNode> object_node;
    std::vector<std::shared_ptr<WeaveExpressionNode>> inputs;
};

}


