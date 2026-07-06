#include "WaveGraph/WeaveNodeRegistry.hpp"

#include <string>
#include <utility>

namespace Mirage::EditorCore::WaveGraph {

namespace {

std::vector<std::string_view> SplitPath(const std::string_view path) {
    std::vector<std::string_view> segments;
    std::size_t segmentStart = 0;

    while (segmentStart < path.size()) {
        const std::size_t delimiterPosition = path.find('.', segmentStart);
        if (delimiterPosition == std::string_view::npos) {
            segments.push_back(path.substr(segmentStart));
            break;
        }

        if (delimiterPosition > segmentStart) {
            segments.push_back(path.substr(segmentStart, delimiterPosition - segmentStart));
        }
        segmentStart = delimiterPosition + 1;
    }

    return segments;
}

} // namespace

WeaveNodeRegistry::WeaveNodeRegistry(std::shared_ptr<WeaveGraphManager> graphManager)
    : graphManager_(std::move(graphManager))
    , rootGroup_{.name = "root", .subGroups = {}, .nodes = {}} {
}

void WeaveNodeRegistry::RegisterProvider(std::shared_ptr<IWeaveNodeProvider> provider) {
    if (provider == nullptr) {
        return;
    }

    providers_.push_back(std::move(provider));
}

void WeaveNodeRegistry::Rebuild() {
    rootGroup_.subGroups.clear();
    rootGroup_.nodes.clear();

    for (const auto& provider : providers_) {
        if (provider == nullptr) {
            continue;
        }

        MergeGroup(rootGroup_, provider->GetNodeGroup(graphManager_));
    }
}

const WeaveNodeGroupInfo& WeaveNodeRegistry::GetRootGroup() const noexcept {
    return rootGroup_;
}

const WeaveNodeGroupInfo* WeaveNodeRegistry::FindGroupByPath(const std::string_view path) const noexcept {
    if (path.empty()) {
        return &rootGroup_;
    }

    const std::vector<std::string_view> segments = SplitPath(path);
    const WeaveNodeGroupInfo* currentGroup = &rootGroup_;
    for (const std::string_view segment : segments) {
        currentGroup = currentGroup->FindSubGroupByName(segment);
        if (currentGroup == nullptr) {
            return nullptr;
        }
    }

    return currentGroup;
}

const WeaveNodeInfo* WeaveNodeRegistry::FindNodeByPath(const std::string_view path) const noexcept {
    const std::vector<std::string_view> segments = SplitPath(path);
    if (segments.empty()) {
        return nullptr;
    }

    const WeaveNodeGroupInfo* currentGroup = &rootGroup_;
    for (std::size_t index = 0; index + 1 < segments.size(); ++index) {
        currentGroup = currentGroup->FindSubGroupByName(segments[index]);
        if (currentGroup == nullptr) {
            return nullptr;
        }
    }

    return currentGroup->FindNodeByName(segments.back());
}

void WeaveNodeRegistry::MergeGroup(WeaveNodeGroupInfo& target, const WeaveNodeGroupInfo& source) {
    WeaveNodeGroupInfo* targetGroup = target.FindSubGroupByName(source.name);
    if (targetGroup == nullptr) {
        target.subGroups.push_back(source);
        return;
    }

    for (const WeaveNodeInfo& sourceNode : source.nodes) {
        WeaveNodeInfo* targetNode = targetGroup->FindNodeByName(sourceNode.name);
        if (targetNode == nullptr) {
            targetGroup->nodes.push_back(sourceNode);
            continue;
        }

        *targetNode = sourceNode;
    }

    for (const WeaveNodeGroupInfo& sourceSubGroup : source.subGroups) {
        MergeGroup(*targetGroup, sourceSubGroup);
    }
}

} // namespace Mirage::EditorCore::WaveGraph
