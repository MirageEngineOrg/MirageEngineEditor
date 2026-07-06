#pragma once

#include "Explorer/ExplorerEntry.hpp"

namespace Mirage::EditorQt {

class ExplorerFileEntry final : public ExplorerEntry {
public:
    explicit ExplorerFileEntry(QString name, ExplorerFolderEntry* parentFolder = nullptr);
};

} // namespace Mirage::EditorQt
