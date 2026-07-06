#pragma once

#include "WaveGraph/WeaveGraphManager.hpp"
#include "WaveGraph/WeaveNodeRegistry.hpp"

#include <memory>
#include <unordered_map>

namespace Mirage::EditorCore {

class EditorCore {
public:
    static EditorCore& GetInstance() noexcept;

    [[nodiscard]] std::shared_ptr<WaveGraph::WeaveGraphManager> GetWeaveGraphManager() const;
    [[nodiscard]] std::shared_ptr<WaveGraph::WeaveNodeRegistry> GetNodeRegistry() const;
private:
    EditorCore();
    std::shared_ptr<WaveGraph::WeaveGraphManager> graph_manager_;
    std::shared_ptr<WaveGraph::WeaveNodeRegistry> nodeRegistry_;
};

} // namespace Mirage::EditorCore
