#pragma once

#include <QFrame>

class QFile;
class QStackedWidget;
class QWidget;

namespace Mirage::EditorQt {

class DockingTabBar;
class DockingWidget;

class DockingWindow final : public QFrame {
public:
    explicit DockingWindow(QWidget* parent = nullptr);

#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif

    [[nodiscard]] bool IsFloatingWindow() const noexcept;
    int addDockWidgetAt(DockingWidget* dockingWidget, int index);
    void SetChromeVisible(bool visible);
    void addDockWidget(DockingWidget* dockingWidget);
    void TransferDockWidgetTo(int from, DockingTabBar *targetTabBar, int to_idx,
                              const QPoint &globalPosition, const QPoint &grabOffset,
                              bool continueDrag);

private slots:
    void OnCurrentTabChanged(int index);
    void OnTabDetachRequested(int index, QPoint globalPosition);
    void OnTabCloseRequested(int index);
    void OnTabMoveRequested(int from_idx, int to_idx);
    void OnTabTransferRequested(int from_idx, Mirage::EditorQt::DockingTabBar *targetTabBar, int to_idx);
private:
    void ApplyStyles();
    void RefreshWindowTitle();
    DockingWidget* TakeDockWidget(int index);

    DockingTabBar* tabBar_ {nullptr};
    QWidget* toolBarSurface_ {nullptr};
    QStackedWidget* stack_ {nullptr};
};

} // namespace Mirage::EditorQt
