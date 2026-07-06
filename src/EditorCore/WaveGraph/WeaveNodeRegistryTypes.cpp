#include "WaveGraph/WeaveNodeRegistryTypes.hpp"

#include <algorithm>
#include <ranges>

namespace Mirage::EditorCore::WaveGraph {

WeaveGraphNodeKind WeaveNodeInputDescriptor::GetKind() const noexcept {
    if (getKind == nullptr) {
        return WeaveGraphNodeKinds::Unknown;
    }

    return getKind();
}


WeaveGraphNodeKind WeaveNodeOutputDescriptor::GetKind() const noexcept {
    if (getKind == nullptr) {
        return WeaveGraphNodeKinds::Unknown;
    }

    return getKind();
}

const WeaveNodeGroupInfo* WeaveNodeGroupInfo::FindSubGroupByName(const std::string_view subGroupName) const noexcept {
    const auto it = std::ranges::find(subGroups, subGroupName, &WeaveNodeGroupInfo::name);
    return it == subGroups.end() ? nullptr : &(*it);
}

WeaveNodeGroupInfo* WeaveNodeGroupInfo::FindSubGroupByName(const std::string_view subGroupName) noexcept {
    const auto it = std::ranges::find(subGroups, subGroupName, &WeaveNodeGroupInfo::name);
    return it == subGroups.end() ? nullptr : &(*it);
}

const WeaveNodeInfo* WeaveNodeGroupInfo::FindNodeByName(const std::string_view nodeName) const noexcept {
    const auto it = std::ranges::find(nodes, nodeName, &WeaveNodeInfo::name);
    return it == nodes.end() ? nullptr : &(*it);
}

WeaveNodeInfo* WeaveNodeGroupInfo::FindNodeByName(const std::string_view nodeName) noexcept {
    const auto it = std::ranges::find(nodes, nodeName, &WeaveNodeInfo::name);
    return it == nodes.end() ? nullptr : &(*it);
}

} // namespace Mirage::EditorCore::WaveGraph
