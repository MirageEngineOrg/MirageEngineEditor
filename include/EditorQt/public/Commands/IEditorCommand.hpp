#pragma once

namespace Mirage::EditorQt {

class IEditorCommand {
public:
    virtual ~IEditorCommand() = default;

    virtual void Execute() = 0;
    virtual void Undo() = 0;
};

} // namespace Mirage::EditorQt
