#include "Docking/DockingWidget.hpp"

#include <QString>

namespace Mirage::EditorQt {

DockingWidget::DockingWidget(QString title, bool closable, QWidget* parent)
    : QWidget(parent)
    , title_(std::move(title))
    , closable_(closable) {
}

QWidget* DockingWidget::CreateToolBarWidget(QWidget* parent) {
    Q_UNUSED(parent);
    return nullptr;
}

bool DockingWidget::IsClosable() const noexcept {
    return closable_;
}

const QString& DockingWidget::GetTitle() const noexcept {
    return title_;
}

} // namespace Mirage::EditorQt
