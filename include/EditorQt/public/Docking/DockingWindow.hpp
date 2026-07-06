#pragma once

#include "Docking/DockingTabHost.hpp"

#include <QFrame>

class QFile;
class QHBoxLayout;
class QStackedWidget;
class QWidget;

namespace Mirage::EditorQt {

class DockingTabBar;
class DockingWidget;
class DockingWorkspacePage;

class DockingWindow : public QFrame, public DockingTabHost {
public:
    explicit DockingWindow(QWidget* parent = nullptr);

#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif

    [[nodiscard]] bool IsFloatingWindow() const noexcept;
    [[nodiscard]] bool IsEmbeddedInWorkspace() const noexcept;
    int addDockWidgetAt(DockingWidget* dockingWidget, int index);
    void SetChromeVisible(bool visible);
    void SetTabBarVisible(bool visible);
    void SetToolBarVisible(bool visible);
    void addDockWidget(DockingWidget* dockingWidget);
    void MakeFloating(const QPoint& globalPosition, const QSize& size);
    bool DetachFromWorkspaceToFloating(const QPoint& globalPosition);
    int InsertDockWidgetIntoTabBar(
        DockingTabBar* targetTabBar,
        DockingWidget* dockingWidget,
        int index
    ) override;
    void TransferDockWidgetTo(int from, DockingTabBar *targetTabBar, int to_idx,
                              const QPoint &globalPosition, const QPoint &grabOffset,
                              bool continueDrag);
    bool TransferDockWidgetToWorkspace(
        int from,
        DockingWorkspacePage* targetWorkspacePage,
        const QPoint& globalPosition
    );

protected:
    explicit DockingWindow(DockingTabBar* externalTabBar, QWidget* parent = nullptr);

private slots:
    void OnCurrentTabChanged(int index);
    void OnTabDetachRequested(int index, QPoint globalPosition);
    void OnTabCloseRequested(int index);
    void OnTabMoveRequested(int from_idx, int to_idx);
    void OnTabTransferRequested(int from_idx, Mirage::EditorQt::DockingTabBar *targetTabBar, int to_idx);
private:
    void ApplyStyles();
    void DestroyIfEmpty();
    [[nodiscard]] DockingWorkspacePage* FindWorkspacePage() const noexcept;
    void ConnectTabBarSignals();
    void RefreshWindowTitle();
    void RefreshToolBar();
    DockingWidget* TakeDockWidget(int index);

    DockingTabBar* tabBar_ {nullptr};
    QWidget* toolBarSurface_ {nullptr};
    QHBoxLayout* toolBarLayout_ {nullptr};
    QStackedWidget* stack_ {nullptr};
    QWidget* currentToolBarWidget_ {nullptr};
    bool ownsTabBar_ {true};
};

} // namespace Mirage::EditorQt
