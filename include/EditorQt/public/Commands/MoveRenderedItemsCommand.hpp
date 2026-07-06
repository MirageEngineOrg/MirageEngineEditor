#pragma once

#include "Commands/IEditorCommand.hpp"
#include "WeaveCanvasBase/WeaveCanvasBase.h"

#include <functional>

namespace Mirage::EditorQt {

class MoveRenderedItemsCommand final : public IEditorCommand {
public:
    using ApplyRectFunction = std::function<void(RenderedItemID, const QRectF&)>;

    MoveRenderedItemsCommand(
        RenderedItemsMovementSession movementSession,
        ApplyRectFunction applyRectFunction
    );

    void Execute() override;
    void Undo() override;

private:
    void ApplyRects(bool useAfterRects);

    RenderedItemsMovementSession movementSession_ {};
    ApplyRectFunction applyRectFunction_ {};
};

} // namespace Mirage::EditorQt
