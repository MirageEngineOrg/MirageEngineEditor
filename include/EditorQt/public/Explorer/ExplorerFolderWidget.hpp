#pragma once

#include "Explorer/ExplorerItemWidget.hpp"

namespace Mirage::EditorQt {

class ExplorerFolderWidget final : public ExplorerItemWidget {
public:
    explicit ExplorerFolderWidget(QString name, QWidget* parent = nullptr);
};

} // namespace Mirage::EditorQt
