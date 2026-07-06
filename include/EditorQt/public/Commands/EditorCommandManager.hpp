#pragma once

#include "Commands/IEditorCommand.hpp"

#include <memory>
#include <vector>

namespace Mirage::EditorQt {

class EditorCommandManager final {
public:
    void ExecuteCommand(std::unique_ptr<IEditorCommand> command);
    void PushExecutedCommand(std::unique_ptr<IEditorCommand> command);
    void Undo();
    void Redo();

    [[nodiscard]] bool CanUndo() const noexcept;
    [[nodiscard]] bool CanRedo() const noexcept;

private:
    std::vector<std::unique_ptr<IEditorCommand>> undoStack_ {};
    std::vector<std::unique_ptr<IEditorCommand>> redoStack_ {};
};

} // namespace Mirage::EditorQt
