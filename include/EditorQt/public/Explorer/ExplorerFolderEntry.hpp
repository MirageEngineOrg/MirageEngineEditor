#pragma once

#include "Explorer/ExplorerEntry.hpp"

#include <QList>

namespace Mirage::EditorQt {

class ExplorerFileEntry;

class ExplorerFolderEntry final : public ExplorerEntry {
public:
    explicit ExplorerFolderEntry(QString name, ExplorerFolderEntry* parentFolder = nullptr);

    [[nodiscard]] ExplorerFolderEntry* AddFolder(QString name);
    [[nodiscard]] ExplorerFileEntry* AddFile(QString name);
    [[nodiscard]] bool HasChildNamed(const QString& name) const noexcept;
    [[nodiscard]] const QList<ExplorerEntry*>& GetChildren() const noexcept;

private:
    QList<ExplorerEntry*> children_ {};
};

} // namespace Mirage::EditorQt
