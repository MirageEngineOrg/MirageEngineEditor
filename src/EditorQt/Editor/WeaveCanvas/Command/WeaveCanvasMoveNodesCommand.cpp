#include "Editor/WeaveCanvas/Command/WeaveCanvasMoveNodesCommand.h"

#include <utility>

namespace Mirage::EditorQt {

WeaveCanvasMoveNodesCommand::WeaveCanvasMoveNodesCommand(
    WeaveCanvasMovementSession movementSession,
    ApplyRectFunction applyRectFunction
)
    : movementSession_(std::move(movementSession))
    , applyRectFunction_(std::move(applyRectFunction)) {
}

void WeaveCanvasMoveNodesCommand::Execute() {
    ApplyRects(true);
}

void WeaveCanvasMoveNodesCommand::Undo() {
    ApplyRects(false);
}

void WeaveCanvasMoveNodesCommand::ApplyRects(const bool useAfterRects) {
    if (!applyRectFunction_) {
        return;
    }

    for (const WeaveCanvasNodeGeometryChange& geometryChange : movementSession_.nodeGeometryChanges) {
        applyRectFunction_(
            geometryChange.nodeId,
            useAfterRects ? geometryChange.afterRect : geometryChange.beforeRect
        );
    }
}

} // namespace Mirage::EditorQt
