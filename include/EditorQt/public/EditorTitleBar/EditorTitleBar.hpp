#pragma once

#include <QPixmap>
#include <QWidget>

class QEvent;
class QFrame;
class QHBoxLayout;
class QLabel;
class QObject;
class QPushButton;

namespace Mirage::EditorQt {

class EditorTitleBar final : public QWidget {
public:
    explicit EditorTitleBar(QWidget* parent = nullptr);

    [[nodiscard]] bool IsDragZone(const QPoint& localPosition) const;

private:
    bool eventFilter(QObject* watched, QEvent* event) override;

    void ApplyStyles();
    void BuildMenuBar();
    void BuildViewportTitleBar();
    void ConnectWindowControls();

    [[nodiscard]] QFrame* CreateStripe(const char* objectName, int height);
    [[nodiscard]] QWidget* CreateLogoArea();
    [[nodiscard]] QPushButton* CreateMenuButton(const QString& text, int width);
    [[nodiscard]] QPushButton* CreateWindowControlButton(
        const QString& text,
        const QString& objectName,
        int width,
        int fontSize
    );
    [[nodiscard]] QLabel* CreateLogoLabel(const QPixmap& logoPixmap);

    QWidget* logoArea_ {nullptr};
    QFrame* menuBarBackground_ {nullptr};
    QFrame* viewportTitleBarBackground_ {nullptr};
    QHBoxLayout* menuBarLayout_ {nullptr};
    QLabel* logoLabel_ {nullptr};
    QPushButton* fileButton_ {nullptr};
    QPushButton* editButton_ {nullptr};
    QPushButton* windowButton_ {nullptr};
    QPushButton* minimizeButton_ {nullptr};
    QPushButton* maximizeButton_ {nullptr};
    QPushButton* closeButton_ {nullptr};
};

} // namespace Mirage::EditorQt
