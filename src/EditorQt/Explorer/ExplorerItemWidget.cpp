#include "Explorer/ExplorerItemWidget.hpp"

#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace Mirage::EditorQt {

ExplorerItemWidget::ExplorerItemWidget(QString name, QString colorHex, QWidget* parent)
    : QFrame(parent) {
    setFrameShape(QFrame::NoFrame);
    setObjectName("ExplorerItemTile");
    setStyleSheet(
        "#ExplorerItemTile {"
        "  border-radius: 6px;"
        "  background-color: transparent;"
        "}"
    );

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(8);

    iconSwatch_ = new QWidget(this);
    iconSwatch_->setFixedSize(36, 36);
    iconSwatch_->setStyleSheet(
        QString("background-color: %1; border-radius: 4px;").arg(colorHex)
    );

    nameLabel_ = new QLabel(std::move(name), this);
    nameLabel_->setWordWrap(true);
    nameLabel_->setAlignment(Qt::AlignCenter);

    nameLineEdit_ = new QLineEdit(nameLabel_->text(), this);
    nameLineEdit_->setVisible(false);
    nameLineEdit_->installEventFilter(this);

    rootLayout->addWidget(iconSwatch_, 0, Qt::AlignHCenter);
    rootLayout->addWidget(nameLabel_);
    rootLayout->addWidget(nameLineEdit_);

    connect(nameLineEdit_, &QLineEdit::returnPressed, this, &ExplorerItemWidget::EditingSubmitted);
    connect(nameLineEdit_, &QLineEdit::textChanged, this, &ExplorerItemWidget::EditingTextChanged);

    RefreshStyle();
}

void ExplorerItemWidget::SetSelected(const bool selected) {
    isSelected_ = selected;
    RefreshStyle();
}

void ExplorerItemWidget::SetInvalid(const bool invalid) {
    isInvalid_ = invalid;
    RefreshStyle();
}

void ExplorerItemWidget::StartEditing() {
    nameLineEdit_->setText(nameLabel_->text());
    nameLabel_->setVisible(false);
    nameLineEdit_->setVisible(true);
    nameLineEdit_->setFocus(Qt::OtherFocusReason);
    nameLineEdit_->selectAll();
}

void ExplorerItemWidget::StopEditing() {
    nameLineEdit_->setVisible(false);
    nameLabel_->setVisible(true);
}

void ExplorerItemWidget::SetName(QString name) {
    nameLabel_->setText(std::move(name));
    nameLineEdit_->setText(nameLabel_->text());
}

QString ExplorerItemWidget::GetEditedName() const {
    return nameLineEdit_->isVisible() ? nameLineEdit_->text() : nameLabel_->text();
}

bool ExplorerItemWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == nameLineEdit_ && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            emit EditingCancelled();
            return true;
        }
    }

    return QFrame::eventFilter(watched, event);
}

void ExplorerItemWidget::RefreshStyle() {
    const QString backgroundColor =
        isSelected_ ? "rgba(128, 128, 128, 120)" : "transparent";
    const QString borderStyle =
        isInvalid_ ? "2px solid rgb(220, 60, 60)" : "2px solid transparent";

    setStyleSheet(
        QString(
            "#ExplorerItemTile {"
            "  border-radius: 6px;"
            "  background-color: %1;"
            "  border: %2;"
            "}"
        ).arg(backgroundColor, borderStyle)
    );
}

} // namespace Mirage::EditorQt
