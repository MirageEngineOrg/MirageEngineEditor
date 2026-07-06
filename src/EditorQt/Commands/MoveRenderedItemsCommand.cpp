#include "Commands/MoveRenderedItemsCommand.hpp"

#include <utility>

namespace Mirage::EditorQt {

MoveRenderedItemsCommand::MoveRenderedItemsCommand(
    RenderedItemsMovementSession movementSession,
    ApplyRectFunction applyRectFunction
)
    : movementSession_(std::move(movementSession))
    , applyRectFunction_(std::move(applyRectFunction)) {
}

void MoveRenderedItemsCommand::Execute() {
    ApplyRects(true);
}

void MoveRenderedItemsCommand::Undo() {
    ApplyRects(false);
}

void MoveRenderedItemsCommand::ApplyRects(const bool useAfterRects) {
    if (!applyRectFunction_) {
        return;
    }

    for (const RenderedItemGeometryChange& geometryChange : movementSession_.itemGeometryChanges) {
        applyRectFunction_(
            geometryChange.renderedItemID,
            useAfterRects ? geometryChange.afterRect : geometryChange.beforeRect
        );
    }
}

} // namespace Mirage::EditorQt
