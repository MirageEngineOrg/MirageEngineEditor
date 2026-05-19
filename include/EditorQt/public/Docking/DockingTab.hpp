#pragma once

#include <QFrame>

class QLabel;
class QMouseEvent;
class QToolButton;
class QString;

namespace Mirage::EditorQt {

class DockingTab final : public QFrame {
    Q_OBJECT

public:
    explicit DockingTab(const QString& title, bool closable, QWidget* parent = nullptr);

    [[nodiscard]] bool IsClosable() const noexcept;
    [[nodiscard]] bool IsDragZone(const QPoint& localPosition) const;
    void SetActive(bool active);

signals:
    void Activated();
    void CloseRequested();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QLabel* titleLabel_ {nullptr};
    QToolButton* closeButton_ {nullptr};
    bool closable_;
};

} // namespace Mirage::EditorQt
