#pragma once

#include <QFrame>

class QLabel;
class QLineEdit;

namespace Mirage::EditorQt {

class ExplorerItemWidget : public QFrame {
    Q_OBJECT

public:
    explicit ExplorerItemWidget(
        QString name,
        QString colorHex,
        QWidget* parent = nullptr
    );

    void SetSelected(bool selected);
    void SetInvalid(bool invalid);
    void StartEditing();
    void StopEditing();
    void SetName(QString name);
    [[nodiscard]] QString GetEditedName() const;

signals:
    void EditingSubmitted();
    void EditingCancelled();
    void EditingTextChanged(const QString& text);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void RefreshStyle();

    QLabel* nameLabel_ {nullptr};
    QWidget* iconSwatch_ {nullptr};
    QLineEdit* nameLineEdit_ {nullptr};
    bool isSelected_ {false};
    bool isInvalid_ {false};
};

} // namespace Mirage::EditorQt
