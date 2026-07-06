#pragma once

#include "WaveGraph/WeaveGraphNode.hpp"

using WeaveValueKind = uint64_t;

namespace  WeaveValueKindValues {
    constexpr WeaveValueKind Unknown = 0;
    constexpr WeaveValueKind Bool = 1;
    constexpr WeaveValueKind Integer = 2;
    constexpr WeaveValueKind Float = 3;
    constexpr WeaveValueKind Double = 4;
    constexpr WeaveValueKind String = 5;
}

namespace Mirage::EditorCore::WaveGraph {

class WeaveExpressionNode : public WeaveGraphNode {
public:
    [[nodiscard]] virtual WeaveValueKind GetValueKind() const noexcept = 0;
    explicit WeaveExpressionNode(WeaveNodeId id) noexcept;
};

} // namespace Mirage::EditorCore::WaveGraph
