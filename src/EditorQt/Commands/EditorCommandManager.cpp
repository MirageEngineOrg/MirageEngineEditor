#include "Commands/EditorCommandManager.hpp"

#include <utility>

namespace Mirage::EditorQt {

void EditorCommandManager::ExecuteCommand(std::unique_ptr<IEditorCommand> command) {
    if (!command) {
        return;
    }

    command->Execute();
    undoStack_.push_back(std::move(command));
    redoStack_.clear();
}

void EditorCommandManager::PushExecutedCommand(std::unique_ptr<IEditorCommand> command) {
    if (!command) {
        return;
    }

    undoStack_.push_back(std::move(command));
    redoStack_.clear();
}

void EditorCommandManager::Undo() {
    if (undoStack_.empty()) {
        return;
    }

    std::unique_ptr<IEditorCommand> command = std::move(undoStack_.back());
    undoStack_.pop_back();
    command->Undo();
    redoStack_.push_back(std::move(command));
}

void EditorCommandManager::Redo() {
    if (redoStack_.empty()) {
        return;
    }

    std::unique_ptr<IEditorCommand> command = std::move(redoStack_.back());
    redoStack_.pop_back();
    command->Execute();
    undoStack_.push_back(std::move(command));
}

bool EditorCommandManager::CanUndo() const noexcept {
    return !undoStack_.empty();
}

bool EditorCommandManager::CanRedo() const noexcept {
    return !redoStack_.empty();
}

} // namespace Mirage::EditorQt
