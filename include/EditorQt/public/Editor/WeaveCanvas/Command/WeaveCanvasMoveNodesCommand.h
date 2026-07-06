#pragma once

#include "Commands/IEditorCommand.hpp"
#include "Editor/WeaveCanvas/Command/WeaveCanvasMovementSession.h"

#include <functional>

namespace Mirage::EditorQt {

class WeaveCanvasMoveNodesCommand final : public IEditorCommand {
public:
    using ApplyRectFunction = std::function<void(Mirage::EditorCore::WaveGraph::WeaveNodeId,
        const QRectF&
    )>;

    WeaveCanvasMoveNodesCommand(
        WeaveCanvasMovementSession movementSession,
        ApplyRectFunction applyRectFunction
    );

    void Execute() override;
    void Undo() override;

private:
    void ApplyRects(bool useAfterRects);

    WeaveCanvasMovementSession movementSession_ {};
    ApplyRectFunction applyRectFunction_ {};
};

} // namespace Mirage::EditorQt
