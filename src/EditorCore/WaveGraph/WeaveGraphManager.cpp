#include "WaveGraph/WeaveGraphManager.hpp"

namespace Mirage::EditorCore::WaveGraph {

WeaveGraphId WeaveGraphManager::CreateGraph() {
    const WeaveGraphId graphId = GenerateGraphId();
    graphs_.emplace(graphId, std::make_shared<WeaveGraph>());
    return graphId;
}

std::shared_ptr<WeaveGraph> WeaveGraphManager::GetGraph(const WeaveGraphId graphId) const {
    const auto it = graphs_.find(graphId);
    if (it == graphs_.end()) {
        return nullptr;
    }

    return it->second;
}

bool WeaveGraphManager::ContainsGraph(const WeaveGraphId graphId) const noexcept {
    return graphs_.contains(graphId);
}

bool WeaveGraphManager::DeleteGraph(const WeaveGraphId graphId) noexcept {
    return graphs_.erase(graphId) > 0;
}

WeaveGraphId WeaveGraphManager::GenerateGraphId() noexcept {
    return nextGraphId_++;
}

} // namespace Mirage::EditorCore::WaveGraph
