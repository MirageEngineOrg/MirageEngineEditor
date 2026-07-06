#pragma once

#include "WaveGraph/WeaveGraphManager.hpp"
#include "WaveGraph/WeaveNodeRegistryTypes.hpp"

namespace Mirage::EditorCore::WaveGraph {

class IWeaveNodeProvider {
public:
    virtual ~IWeaveNodeProvider() = default;

    IWeaveNodeProvider(const IWeaveNodeProvider&) = delete;
    IWeaveNodeProvider& operator=(const IWeaveNodeProvider&) = delete;
    IWeaveNodeProvider(IWeaveNodeProvider&&) = delete;
    IWeaveNodeProvider& operator=(IWeaveNodeProvider&&) = delete;

    [[nodiscard]] virtual WeaveNodeGroupInfo GetNodeGroup(const std::shared_ptr<WeaveGraphManager>& graphManager) const = 0;

protected:
    IWeaveNodeProvider() = default;
};

} // namespace Mirage::EditorCore::WaveGraph
