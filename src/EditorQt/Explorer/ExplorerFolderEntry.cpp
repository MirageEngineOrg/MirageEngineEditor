#include "Explorer/ExplorerFolderEntry.hpp"

#include "Explorer/ExplorerFileEntry.hpp"

namespace Mirage::EditorQt {

ExplorerFolderEntry::ExplorerFolderEntry(QString name, ExplorerFolderEntry* parentFolder)
    : ExplorerEntry(std::move(name), Kind::Folder, parentFolder) {
}

ExplorerFolderEntry* ExplorerFolderEntry::AddFolder(QString name) {
    auto* folderEntry = new ExplorerFolderEntry(std::move(name), this);
    children_.append(folderEntry);
    emit ContentChanged();
    return folderEntry;
}

ExplorerFileEntry* ExplorerFolderEntry::AddFile(QString name) {
    auto* fileEntry = new ExplorerFileEntry(std::move(name), this);
    children_.append(fileEntry);
    emit ContentChanged();
    return fileEntry;
}

bool ExplorerFolderEntry::HasChildNamed(const QString& name) const noexcept {
    for (const ExplorerEntry* child : children_) {
        if (child != nullptr && child->GetName().compare(name, Qt::CaseSensitive) == 0) {
            return true;
        }
    }

    return false;
}

const QList<ExplorerEntry*>& ExplorerFolderEntry::GetChildren() const noexcept {
    return children_;
}

} // namespace Mirage::EditorQt
