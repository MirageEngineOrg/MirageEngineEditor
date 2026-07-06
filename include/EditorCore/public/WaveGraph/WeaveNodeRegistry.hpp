#pragma once

#include "WaveGraph/NodeProvider/IWeaveNodeProvider.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace Mirage::EditorCore::WaveGraph {

class WeaveNodeRegistry {
public:
    explicit WeaveNodeRegistry(std::shared_ptr<WeaveGraphManager> graphManager);

    void RegisterProvider(std::shared_ptr<IWeaveNodeProvider> provider);
    void Rebuild();

    [[nodiscard]] const WeaveNodeGroupInfo& GetRootGroup() const noexcept;
    [[nodiscard]] const WeaveNodeGroupInfo* FindGroupByPath(std::string_view path) const noexcept;
    [[nodiscard]] const WeaveNodeInfo* FindNodeByPath(std::string_view path) const noexcept;

private:
    static void MergeGroup(WeaveNodeGroupInfo& target, const WeaveNodeGroupInfo& source);

    std::shared_ptr<WeaveGraphManager> graphManager_;
    std::vector<std::shared_ptr<IWeaveNodeProvider>> providers_ {};
    WeaveNodeGroupInfo rootGroup_ {};
};

} // namespace Mirage::EditorCore::WaveGraph
