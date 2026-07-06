#pragma once

#include "WaveGraph/Nodes/WeaveExpressionNode.hpp"

#include <cstdint>
#include <string>
#include <variant>

namespace Mirage::EditorCore::WaveGraph {


using WeaveConstantValue = std::variant<std::string, bool, std::int64_t, double, float>;

class WeaveConstantNode : public WeaveExpressionNode {
public:
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;
    [[nodiscard]] const WeaveConstantValue& GetValue() const noexcept;
    [[nodiscard]] WeaveValueKind GetValueKind() const noexcept override;

    WeaveConstantNode(WeaveNodeId id, WeaveConstantValue value);

    WeaveConstantValue value_;
};

} // namespace Mirage::EditorCore::WaveGraph
