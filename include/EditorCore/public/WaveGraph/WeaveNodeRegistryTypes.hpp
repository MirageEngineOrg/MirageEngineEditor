#pragma once

#include "WaveGraph/WeaveGraphNode.hpp"
#include "WaveGraph/WeaveGraphTypes.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Mirage::EditorCore::WaveGraph {


class InputConnectionHandler {
public:
    virtual ~InputConnectionHandler() = default;
    virtual bool ValidateConnection(std::shared_ptr<WeaveGraphNode> targetNode) = 0;
    virtual bool Connect(std::shared_ptr<WeaveGraphNode> targetNode) = 0;
};

struct WeaveNodeInputDescriptor {
    std::string name;
    std::unique_ptr<InputConnectionHandler> connection_handler;
    std::function<WeaveGraphNodeKind()> getKind;

    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept;
};


struct WeaveNodeOutputDescriptor {
    std::string name;
    std::function<WeaveGraphNodeKind()> getKind;

    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept;
};

struct WeaveCreatedNodeInfo {
    WeaveNodeId nodeId {0};
    std::vector<WeaveNodeInputDescriptor> inputs;
    std::vector<WeaveNodeOutputDescriptor> outputs;
};

using WeaveNodeConstructor = std::function<WeaveCreatedNodeInfo(WeaveGraphId)>;

struct WeaveNodeInfo {
    std::string name;
    std::string description;
    WeaveNodeConstructor constructor;
};

struct WeaveNodeGroupInfo {
    std::string name;
    std::vector<WeaveNodeGroupInfo> subGroups;
    std::vector<WeaveNodeInfo> nodes;

    [[nodiscard]] const WeaveNodeGroupInfo* FindSubGroupByName(std::string_view subGroupName) const noexcept;
    [[nodiscard]] WeaveNodeGroupInfo* FindSubGroupByName(std::string_view subGroupName) noexcept;
    [[nodiscard]] const WeaveNodeInfo* FindNodeByName(std::string_view nodeName) const noexcept;
    [[nodiscard]] WeaveNodeInfo* FindNodeByName(std::string_view nodeName) noexcept;
};

} // namespace Mirage::EditorCore::WaveGraph
