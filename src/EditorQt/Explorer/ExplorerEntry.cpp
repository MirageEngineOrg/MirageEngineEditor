#include "Explorer/ExplorerEntry.hpp"

#include "Explorer/ExplorerFolderEntry.hpp"

namespace Mirage::EditorQt {

ExplorerEntry::ExplorerEntry(QString name, const Kind kind, ExplorerFolderEntry* parentFolder)
    : QObject(parentFolder)
    , name_(std::move(name))
    , kind_(kind)
    , parentFolder_(parentFolder) {
}

ExplorerEntry::Kind ExplorerEntry::GetKind() const noexcept {
    return kind_;
}

const QString& ExplorerEntry::GetName() const noexcept {
    return name_;
}

ExplorerFolderEntry* ExplorerEntry::GetParentFolder() const noexcept {
    return parentFolder_;
}

} // namespace Mirage::EditorQt
