#pragma once

#include "WaveGraph/WeaveGraphTypes.hpp"

#include <QRectF>
#include <QVector>

namespace Mirage::EditorQt {

struct WeaveCanvasNodeGeometryChange {
    Mirage::EditorCore::WaveGraph::WeaveNodeId nodeId{0};
    QRectF beforeRect {};
    QRectF afterRect {};
};

struct WeaveCanvasMovementSession {
    QVector<WeaveCanvasNodeGeometryChange> nodeGeometryChanges {};

    [[nodiscard]] bool IsEmpty() const noexcept {
        return nodeGeometryChanges.isEmpty();
    }
};

} // namespace Mirage::EditorQt
