#include "Docking/DockingWindow.hpp"

#include "Docking/DockingInternal.hpp"
#include "Docking/DockingTabBar.hpp"
#include "Docking/DockingWidget.hpp"
#include "Docking/DockingWorkspacePage.hpp"

#include <QByteArray>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>

namespace {

void EnableSnapAndResize(QWidget* widget) {
    auto *const hwnd = reinterpret_cast<HWND>(widget->winId());

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CAPTION;

    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    SetWindowPos(
        hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );
}

} // namespace
#endif

namespace Mirage::EditorQt {

DockingWindow::DockingWindow(QWidget* parent)
    : DockingWindow(nullptr, parent) {
}

DockingWindow::DockingWindow(DockingTabBar* externalTabBar, QWidget* parent)
    : QFrame(parent) {
    if (parent == nullptr) {
        setWindowFlags(Qt::FramelessWindowHint);
    }

    setObjectName("DockingWindowRoot");
    setMinimumSize(0, 0);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    ownsTabBar_ = externalTabBar == nullptr;
    if (ownsTabBar_) {
        tabBar_ = new DockingTabBar(this);
        rootLayout->addWidget(tabBar_);
    } else {
        tabBar_ = externalTabBar;
    }
    tabBar_->SetHost(this);

    toolBarSurface_ = new QWidget(this);
    toolBarSurface_->setObjectName("DockingToolBarSurface");
    toolBarSurface_->setFixedHeight(kDockingToolBarHeight);
    toolBarLayout_ = new QHBoxLayout(toolBarSurface_);
    toolBarLayout_->setContentsMargins(12, 0, 12, 0);
    toolBarLayout_->setSpacing(8);
    rootLayout->addWidget(toolBarSurface_);

    stack_ = new QStackedWidget(this);
    stack_->setObjectName("DockingContentSurface");
    rootLayout->addWidget(stack_, 1);

    ConnectTabBarSignals();
    ApplyStyles();

#ifdef Q_OS_WIN
    if (parent == nullptr) {
        EnableSnapAndResize(this);
    }
#endif
}

void DockingWindow::ConnectTabBarSignals() {
    connect(tabBar_, &DockingTabBar::CurrentChanged, this, &DockingWindow::OnCurrentTabChanged);
    connect(tabBar_, &DockingTabBar::TabDetachRequested, this, &DockingWindow::OnTabDetachRequested);
    connect(tabBar_, &DockingTabBar::TabCloseRequested, this, &DockingWindow::OnTabCloseRequested);
    connect(tabBar_, &DockingTabBar::TabMoveRequested, this, &DockingWindow::OnTabMoveRequested);
    connect(tabBar_, &DockingTabBar::TabTransferRequested, this, &DockingWindow::OnTabTransferRequested);
}

#ifdef Q_OS_WIN
bool DockingWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    Q_UNUSED(eventType);

    auto* msg = static_cast<MSG*>(message);

    if (msg->message == WM_NCCALCSIZE) {
        *result = WVR_REDRAW;
        return true;
    }

    if (msg->message == WM_GETMINMAXINFO) {
        auto* minMaxInfo = reinterpret_cast<LPMINMAXINFO>(msg->lParam);

        const qreal dpiScale = devicePixelRatioF();
        minMaxInfo->ptMinTrackSize.x = static_cast<LONG>(minimumWidth() * dpiScale);
        minMaxInfo->ptMinTrackSize.y = static_cast<LONG>(minimumHeight() * dpiScale);

        const HMONITOR monitor = MonitorFromWindow(msg->hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo {};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (GetMonitorInfo(monitor, &monitorInfo) != FALSE) {
            const RECT workArea = monitorInfo.rcWork;
            const RECT monitorArea = monitorInfo.rcMonitor;

            minMaxInfo->ptMaxPosition.x = workArea.left - monitorArea.left;
            minMaxInfo->ptMaxPosition.y = workArea.top - monitorArea.top;
            minMaxInfo->ptMaxSize.x = workArea.right - workArea.left;
            minMaxInfo->ptMaxSize.y = workArea.bottom - workArea.top;

            *result = 0;
            return true;
        }
    }

    if (msg->message == WM_NCHITTEST) {
        constexpr int borderWidth = 8;

        RECT windowRect {};
        GetWindowRect(HWND(winId()), &windowRect);

        const LONG x = GET_X_LPARAM(msg->lParam);
        const LONG y = GET_Y_LPARAM(msg->lParam);

        const int width = windowRect.right - windowRect.left;
        const int height = windowRect.bottom - windowRect.top;

        const QPoint localPosition = mapFromGlobal(QPoint(x, y));

        if (localPosition.x() < borderWidth && localPosition.y() < borderWidth) {
            *result = HTTOPLEFT;
        } else if (localPosition.x() > width - borderWidth
                   && localPosition.y() < borderWidth) {
            *result = HTTOPRIGHT;
        } else if (localPosition.x() < borderWidth
                   && localPosition.y() > height - borderWidth) {
            *result = HTBOTTOMLEFT;
        } else if (localPosition.x() > width - borderWidth
                   && localPosition.y() > height - borderWidth) {
            *result = HTBOTTOMRIGHT;
        } else if (localPosition.x() < borderWidth) {
            *result = HTLEFT;
        } else if (localPosition.x() > width - borderWidth) {
            *result = HTRIGHT;
        } else if (localPosition.y() < borderWidth) {
            *result = HTTOP;
        } else if (localPosition.y() > height - borderWidth) {
            *result = HTBOTTOM;
        } else if (tabBar_ != nullptr) {
            const QPoint tabBarLocalPosition = tabBar_->mapFromGlobal(QPoint(x, y));
            if (tabBar_->rect().contains(tabBarLocalPosition)
                && tabBar_->IsDragZone(tabBarLocalPosition)) {
                *result = HTCAPTION;
            } else {
                *result = HTCLIENT;
            }
        } else {
            *result = HTCLIENT;
        }

        return true;
    }

    return QFrame::nativeEvent(eventType, message, result);
}
#endif

bool DockingWindow::IsFloatingWindow() const noexcept {
    return parentWidget() == nullptr && isWindow();
}

bool DockingWindow::IsEmbeddedInWorkspace() const noexcept {
    return FindWorkspacePage() != nullptr;
}

DockingWorkspacePage* DockingWindow::FindWorkspacePage() const noexcept {
    for (QWidget* ancestor = parentWidget(); ancestor != nullptr; ancestor = ancestor->parentWidget()) {
        if (auto* workspacePage = dynamic_cast<DockingWorkspacePage*>(ancestor); workspacePage != nullptr) {
            return workspacePage;
        }
    }

    return nullptr;
}

void DockingWindow::SetChromeVisible(bool visible) {
    SetTabBarVisible(visible);
    SetToolBarVisible(visible);
}

void DockingWindow::SetTabBarVisible(bool visible) {
    if (ownsTabBar_) {
        tabBar_->setVisible(visible);
    }
}

void DockingWindow::SetToolBarVisible(bool visible) {
    toolBarSurface_->setVisible(visible);
}

void DockingWindow::addDockWidget(DockingWidget* dockingWidget) {
    addDockWidgetAt(dockingWidget, stack_->count());
}

void DockingWindow::MakeFloating(const QPoint& globalPosition, const QSize& size) {
    setParent(nullptr);
    setWindowFlags(Qt::FramelessWindowHint);
    SetChromeVisible(true);
    resize(size);
    move(globalPosition - QPoint(48, 12));
    show();

#ifdef Q_OS_WIN
    EnableSnapAndResize(this);
#endif
}

bool DockingWindow::DetachFromWorkspaceToFloating(const QPoint& globalPosition) {
    DockingWorkspacePage* workspacePage = FindWorkspacePage();
    if (workspacePage == nullptr) {
        return false;
    }

    workspacePage->RemoveDockWindow(this);
    MakeFloating(globalPosition, size());
    return true;
}

int DockingWindow::InsertDockWidgetIntoTabBar(
    DockingTabBar* targetTabBar,
    DockingWidget* dockingWidget,
    int index
) {
    if (targetTabBar != tabBar_) {
        return -1;
    }

    return addDockWidgetAt(dockingWidget, index);
}

int DockingWindow::addDockWidgetAt(DockingWidget* dockingWidget, int index) {
    dockingWidget->setParent(stack_);
    const int boundedIndex = qBound(0, index, stack_->count());
    stack_->insertWidget(boundedIndex, dockingWidget);
    const int tabIndex = tabBar_->InsertTab(
        boundedIndex,
        dockingWidget->GetTitle(),
        dockingWidget->IsClosable()
    );
    tabBar_->SetCurrentIndex(tabIndex);
    stack_->setCurrentIndex(tabIndex);
    RefreshWindowTitle();
    RefreshToolBar();
    return tabIndex;
}

void DockingWindow::TransferDockWidgetTo(const int from, DockingTabBar *targetTabBar,
                                         const int to_idx,
                                         const QPoint &globalPosition, const QPoint &grabOffset,
                                         const bool continueDrag) {
    DockingWidget* dockingWidget = TakeDockWidget(from);
    if (dockingWidget == nullptr || targetTabBar == nullptr) {
        return;
    }

    DockingTabHost* targetHost = targetTabBar->GetHost();
    if (targetHost == nullptr) {
        addDockWidgetAt(dockingWidget, from);
        return;
    }

    targetTabBar->ClearExternalPlaceholder();
    const int insertedIndex = targetHost->InsertDockWidgetIntoTabBar(
        targetTabBar,
        dockingWidget,
        to_idx
    );
    if (insertedIndex < 0) {
        addDockWidgetAt(dockingWidget, from);
        return;
    }

    if (continueDrag) {
        targetTabBar->ContinueTransferredDrag(insertedIndex, globalPosition, grabOffset);
    }

    if (tabBar_->GetTabCount() == 0) {
        DestroyIfEmpty();
        return;
    }

    RefreshWindowTitle();
}

bool DockingWindow::TransferDockWidgetToWorkspace(
    const int from,
    DockingWorkspacePage* targetWorkspacePage,
    const QPoint& globalPosition
) {
    DockingWidget* dockingWidget = TakeDockWidget(from);
    if (dockingWidget == nullptr || targetWorkspacePage == nullptr) {
        return false;
    }

    if (!targetWorkspacePage->InsertDockWidgetAtDropPlaceholder(dockingWidget, globalPosition)) {
        addDockWidgetAt(dockingWidget, from);
        return false;
    }

    if (tabBar_->GetTabCount() == 0) {
        DestroyIfEmpty();
        return true;
    }

    RefreshWindowTitle();
    RefreshToolBar();
    return true;
}

void DockingWindow::OnCurrentTabChanged(int index) {
    stack_->setCurrentIndex(index);
    RefreshWindowTitle();
    RefreshToolBar();
}

void DockingWindow::OnTabDetachRequested(int index, QPoint globalPosition) {
    if (tabBar_->GetTabCount() <= 1 && !IsEmbeddedInWorkspace()) {
        return;
    }

    DockingWidget* dockingWidget = TakeDockWidget(index);
    if (dockingWidget == nullptr) {
        return;
    }

    auto* floatingWindow = new DockingWindow();
    floatingWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    floatingWindow->resize(size());
    floatingWindow->move(globalPosition - QPoint(48, 12));
    floatingWindow->addDockWidget(dockingWidget);
    floatingWindow->show();

    if (QWindow* nativeWindow = floatingWindow->windowHandle(); nativeWindow != nullptr) {
        nativeWindow->startSystemMove();
    }

    if (tabBar_->GetTabCount() == 0) {
        DestroyIfEmpty();
        return;
    }

    RefreshWindowTitle();
    RefreshToolBar();
}

void DockingWindow::OnTabCloseRequested(int index) {
    DockingWidget* dockingWidget = TakeDockWidget(index);
    if (dockingWidget == nullptr || !dockingWidget->IsClosable()) {
        return;
    }
    dockingWidget->deleteLater();

    if (tabBar_->GetTabCount() == 0) {
        DestroyIfEmpty();
        return;
    }

    RefreshWindowTitle();
    RefreshToolBar();
}

void DockingWindow::OnTabMoveRequested(const int from_idx, const int to_idx) {
    if (from_idx == to_idx || from_idx < 0 || to_idx < 0 || from_idx >= stack_->count() || to_idx >= stack_->count()) {
        return;
    }

    QWidget* widget = stack_->widget(from_idx);
    const bool wasCurrent = stack_->currentIndex() == from_idx;
    stack_->removeWidget(widget);
    stack_->insertWidget(to_idx, widget);

    if (wasCurrent || stack_->currentIndex() == from_idx) {
        stack_->setCurrentIndex(to_idx);
    }

    RefreshWindowTitle();
    RefreshToolBar();
}

void DockingWindow::OnTabTransferRequested(const int from_idx, DockingTabBar *targetTabBar,
                                           const int to_idx) {
    TransferDockWidgetTo(from_idx, targetTabBar, to_idx, {}, {}, false);
}

void DockingWindow::ApplyStyles() {
    QFile styleSheetFile(QString::fromUtf8(kDockingStyleSheetPath));
    if (!styleSheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    setStyleSheet(QString::fromUtf8(styleSheetFile.readAll()));
}

void DockingWindow::DestroyIfEmpty() {
    if (tabBar_->GetTabCount() != 0) {
        return;
    }

    if (isWindow()) {
        close();
        return;
    }

    if (DockingWorkspacePage* workspacePage = FindWorkspacePage(); workspacePage != nullptr) {
        workspacePage->RemoveDockWindow(this);
        deleteLater();
    }
}

void DockingWindow::RefreshWindowTitle() {
    const int currentIndex = stack_->currentIndex();
    if (currentIndex < 0) {
        return;
    }

    auto* dockingWidget = dynamic_cast<DockingWidget*>(stack_->widget(currentIndex));
    if (dockingWidget == nullptr) {
        return;
    }

    if (isWindow()) {
        setWindowTitle(dockingWidget->GetTitle());
    }
}

void DockingWindow::RefreshToolBar() {
    while (QLayoutItem* item = toolBarLayout_->takeAt(0)) {
        if (QWidget* widget = item->widget(); widget != nullptr) {
            widget->deleteLater();
        }

        delete item;
    }

    currentToolBarWidget_ = nullptr;

    const int currentIndex = stack_->currentIndex();
    if (currentIndex < 0) {
        toolBarSurface_->setVisible(false);
        return;
    }

    auto* dockingWidget = dynamic_cast<DockingWidget*>(stack_->widget(currentIndex));
    if (dockingWidget == nullptr) {
        toolBarSurface_->setVisible(false);
        return;
    }

    currentToolBarWidget_ = dockingWidget->CreateToolBarWidget(toolBarSurface_);
    if (currentToolBarWidget_ == nullptr) {
        toolBarSurface_->setVisible(false);
        return;
    }

    toolBarLayout_->addWidget(currentToolBarWidget_);
    toolBarLayout_->addStretch(1);
    toolBarSurface_->setVisible(true);
}

DockingWidget* DockingWindow::TakeDockWidget(int index) {
    if (index < 0 || index >= stack_->count()) {
        return nullptr;
    }

    QWidget* widget = stack_->widget(index);
    auto* dockingWidget = dynamic_cast<DockingWidget*>(widget);
    if (dockingWidget == nullptr) {
        return nullptr;
    }

    stack_->removeWidget(widget);
    tabBar_->RemoveTab(index);
    dockingWidget->setParent(nullptr);
    return dockingWidget;
}

} // namespace Mirage::EditorQt
