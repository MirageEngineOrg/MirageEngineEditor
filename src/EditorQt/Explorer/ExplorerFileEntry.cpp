#include "Explorer/ExplorerFileEntry.hpp"

namespace Mirage::EditorQt {

ExplorerFileEntry::ExplorerFileEntry(QString name, ExplorerFolderEntry* parentFolder)
    : ExplorerEntry(std::move(name), Kind::File, parentFolder) {
}

} // namespace Mirage::EditorQt
