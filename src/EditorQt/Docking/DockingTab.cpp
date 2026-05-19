#include "Docking/DockingTab.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QStyle>
#include <QToolButton>

namespace Mirage::EditorQt {

DockingTab::DockingTab(const QString& title, bool closable, QWidget* parent)
    : QFrame(parent)
    , closable_(closable) {
    setObjectName("DockingTab");
    setProperty("active", false);
    setFixedHeight(24);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(13, 0, 9, 0);
    layout->setSpacing(10);

    titleLabel_ = new QLabel(title, this);
    titleLabel_->setObjectName("DockingTabTitle");
    titleLabel_->setProperty("active", false);
    layout->addWidget(titleLabel_);

    layout->addStretch(1);

    closeButton_ = new QToolButton(this);
    closeButton_->setObjectName("DockingTabCloseButton");
    closeButton_->setProperty("active", false);
    closeButton_->setText(QString::fromUtf8("\u00D7"));
    closeButton_->setAutoRaise(true);
    closeButton_->setEnabled(closable_);
    closeButton_->setCursor(closable_ ? Qt::PointingHandCursor : Qt::ArrowCursor);
    layout->addWidget(closeButton_);

    connect(closeButton_, &QToolButton::clicked, this, &DockingTab::CloseRequested);
}

bool DockingTab::IsClosable() const noexcept {
    return closable_;
}

bool DockingTab::IsDragZone(const QPoint& localPosition) const {
    if (QWidget* child = childAt(localPosition); child != nullptr) {
        return qobject_cast<QToolButton*>(child) == nullptr;
    }

    return true;
}

void DockingTab::SetActive(bool active) {
    setProperty("active", active);
    titleLabel_->setProperty("active", active);
    closeButton_->setProperty("active", active);

    style()->unpolish(this);
    style()->polish(this);
    titleLabel_->style()->unpolish(titleLabel_);
    titleLabel_->style()->polish(titleLabel_);
    closeButton_->style()->unpolish(closeButton_);
    closeButton_->style()->polish(closeButton_);

    update();
}

void DockingTab::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit Activated();
    }

    QFrame::mousePressEvent(event);
}

} // namespace Mirage::EditorQt
