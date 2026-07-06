#include "Application/EditorCore.hpp"

#include "WaveGraph/NodeProvider/WeaveBuiltinNodeProvider.hpp"

namespace Mirage::EditorCore {

EditorCore& EditorCore::GetInstance() noexcept {
    static EditorCore instance;
    return instance;
}


std::shared_ptr<WaveGraph::WeaveGraphManager> EditorCore::GetWeaveGraphManager() const {
    return graph_manager_;
}
std::shared_ptr<WaveGraph::WeaveNodeRegistry> EditorCore::GetNodeRegistry() const {
    return nodeRegistry_;
}

EditorCore::EditorCore() {
    graph_manager_ = std::make_shared<WaveGraph::WeaveGraphManager>();
    nodeRegistry_ = std::make_shared<WaveGraph::WeaveNodeRegistry>(graph_manager_);
    nodeRegistry_->RegisterProvider(WaveGraph::WeaveBuiltinNodeProvider::GetInstance());
    nodeRegistry_->Rebuild();
}

} // namespace Mirage::EditorCore
