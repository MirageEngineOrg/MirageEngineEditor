#pragma once

#include "WaveGraph/NodeProvider/IWeaveNodeProvider.hpp"

#include <memory>

namespace Mirage::EditorCore::WaveGraph {

class WeaveBuiltinNodeProvider final : public IWeaveNodeProvider {
public:
    static std::shared_ptr<WeaveBuiltinNodeProvider> GetInstance();

    [[nodiscard]] WeaveNodeGroupInfo GetNodeGroup(const std::shared_ptr<WeaveGraphManager>& graphManager) const override;

private:
    WeaveBuiltinNodeProvider() = default;
};

} // namespace Mirage::EditorCore::WaveGraph
