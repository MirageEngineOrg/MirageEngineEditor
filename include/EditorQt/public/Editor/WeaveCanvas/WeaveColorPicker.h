#pragma once

#include "WaveGraph/WeaveGraphNode.hpp"

#include <QColor>

class WeaveColorPicker final {
public:
    [[nodiscard]] static QColor GetPinColor(Mirage::EditorCore::WaveGraph::WeaveGraphNodeKind kind) noexcept;
    [[nodiscard]] static QColor GetNodeColor(Mirage::EditorCore::WaveGraph::WeaveGraphNodeKind kind) noexcept;
};
