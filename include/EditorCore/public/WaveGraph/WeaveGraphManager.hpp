#pragma once

#include "WaveGraph/WeaveGraph.hpp"

#include <memory>
#include <unordered_map>

namespace Mirage::EditorCore::WaveGraph {

class WeaveGraphManager {
public:
    [[nodiscard]] WeaveGraphId CreateGraph();
    [[nodiscard]] std::shared_ptr<WeaveGraph> GetGraph(WeaveGraphId graphId) const;
    [[nodiscard]] bool ContainsGraph(WeaveGraphId graphId) const noexcept;
    bool DeleteGraph(WeaveGraphId graphId) noexcept;

private:
    [[nodiscard]] WeaveGraphId GenerateGraphId() noexcept;

    std::unordered_map<WeaveGraphId, std::shared_ptr<WeaveGraph>> graphs_ {};
    WeaveGraphId nextGraphId_ {1};
};

} // namespace Mirage::EditorCore::WaveGraph
