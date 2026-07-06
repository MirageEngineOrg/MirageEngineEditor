#include "Explorer/ExplorerFileWidget.hpp"

namespace Mirage::EditorQt {

ExplorerFileWidget::ExplorerFileWidget(QString name, QWidget* parent)
    : ExplorerItemWidget(std::move(name), "#57C7FF", parent) {
}

} // namespace Mirage::EditorQt
