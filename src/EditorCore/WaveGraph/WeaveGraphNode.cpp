#include "WaveGraph/WeaveGraphNode.hpp"

namespace Mirage::EditorCore::WaveGraph {

WeaveGraphNode::WeaveGraphNode(const WeaveNodeId id) noexcept
    : id_(id) {
}

WeaveNodeId WeaveGraphNode::GetId() const noexcept {
    return id_;
}

} // namespace Mirage::EditorCore::WaveGraph
