#include "Explorer/ExplorerFolderWidget.hpp"

namespace Mirage::EditorQt {

ExplorerFolderWidget::ExplorerFolderWidget(QString name, QWidget* parent)
    : ExplorerItemWidget(std::move(name), "#F2B94B", parent) {
}

} // namespace Mirage::EditorQt
