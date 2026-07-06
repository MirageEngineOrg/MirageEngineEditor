#include "WaveGraph/Nodes/Expressions/Constants/WeaveConstantNode.hpp"
#include <utility>

namespace Mirage::EditorCore::WaveGraph {

WeaveConstantNode::WeaveConstantNode(const WeaveNodeId id, WeaveConstantValue value)
    : WeaveExpressionNode(id)
    , value_(std::move(value)) {
}

WeaveGraphNodeKind WeaveConstantNode::GetKind() const noexcept {
    return WeaveGraphNodeKinds::Constant;
}
const WeaveConstantValue& WeaveConstantNode::GetValue() const noexcept {
    return value_;
}

WeaveValueKind WeaveConstantNode::GetValueKind() const noexcept {
    try {
        return std::visit(
        [](const auto& value) -> WeaveValueKind {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, std::string>) {
                return WeaveValueKindValues::String;
            } else if constexpr (std::is_same_v<T, std::int64_t>) {
                return WeaveValueKindValues::Integer;
            } else if constexpr (std::is_same_v<T, double>) {
                return WeaveValueKindValues::Double;
            } else if constexpr (std::is_same_v<T, float>) {
                return WeaveValueKindValues::Float;
            } else if constexpr(std::is_same_v<T, bool>) {
                return WeaveValueKindValues::Bool ;
            }
            return WeaveValueKindValues::Unknown;
        },
        value_);
    }
    catch (...) {
        return WeaveValueKindValues::Unknown;
    }

}


} // namespace Mirage::EditorCore::WaveGraph
