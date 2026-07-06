#include "MainWindow/MainWindow.hpp"

#include "MainWindow/MainWindowInternal.hpp"

#include "Application/EditorApplication.hpp"
#include "Docking/DockingWindow.hpp"
#include "Docking/DockingWorkspacePage.hpp"
#include "Docking/MainDockingWindow.hpp"
#include "Docking/Demo/InspectorDockingWidget.hpp"
#include "Docking/Demo/SceneDockingWidget.hpp"
#include "Editor/WeaveCanvas/WeaveEditor.h"
#include "Explorer/ExplorerFolderEntry.hpp"
#include "Explorer/ExplorerWidget.hpp"

#include <QByteArray>
#include <QHBoxLayout>
#include <QPoint>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>

namespace {

void EnableSnapAndResize(QWidget* widget) {
    const auto hwnd = reinterpret_cast<HWND>(widget->winId());

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

namespace {

DockingWindow* CreateDockingWindowWithWidget(DockingWidget* dockingWidget, QWidget* parent) {
    auto* dockingWindow = new DockingWindow(parent);
    dockingWindow->addDockWidget(dockingWidget);
    return dockingWindow;
}

DockingWorkspacePage* CreateWorkspacePageOne(QWidget* parent) {
    auto* page = new DockingWorkspacePage("Workspace 1", false, Qt::Horizontal, parent);
    page->AddDockWindow(CreateDockingWindowWithWidget(
        new SceneDockingWidget("Scene", false, page),
        page
    ));
    page->AddDockWindow(CreateDockingWindowWithWidget(
        new InspectorDockingWidget("Inspector", false, page),
        page
    ));
    return page;
}

DockingWorkspacePage* CreateWorkspacePageTwo(QWidget* parent) {
    auto* page = new DockingWorkspacePage("Workspace 2", false, Qt::Horizontal, parent);
    page->AddDockWindow(CreateDockingWindowWithWidget(
        new WeaveEditorWidget(false, page),
        page
    ));
    page->AddDockWindow(CreateDockingWindowWithWidget(
        new SceneDockingWidget("Preview", false, page),
        page
    ));
    return page;
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , editorApplication_(Mirage::EditorCore::EditorApplicationConfig{"Mirage Project"}) {
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle(kMainWindowTitle);
    resize(kDefaultWindowWidth, kDefaultWindowHeight);

    auto* centralWidget = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    editorTitleBar_ = new EditorTitleBar(centralWidget);

    mainDockingTabBar_ = new DockingTabBar(editorTitleBar_);
    editorTitleBar_->SetViewportWidget(mainDockingTabBar_);

    mainDockingWindow_ = new MainDockingWindow(mainDockingTabBar_, centralWidget);
    mainDockingWindow_->addDockWidget(CreateWorkspacePageOne(mainDockingWindow_));
    mainDockingWindow_->addDockWidget(CreateWorkspacePageTwo(mainDockingWindow_));

    explorerPanel_ = new QWidget(centralWidget);
    explorerPanel_->setObjectName("BottomExplorerPanel");
    explorerPanel_->setVisible(false);
    explorerPanel_->setMinimumHeight(0);
    explorerPanel_->setMaximumHeight(160);
    auto* explorerLayout = new QVBoxLayout(explorerPanel_);
    explorerLayout->setContentsMargins(16, 12, 16, 12);
    explorerLayout->setSpacing(8);
    auto* explorerWidget = new ExplorerWidget(explorerPanel_);
    auto* assetsFolder = explorerWidget->AddFolder("Assets");
    const auto* playerPrefabFile = assetsFolder->AddFile("Player.prefab");
    Q_UNUSED(playerPrefabFile);
    const auto* enemyPrefabFile = assetsFolder->AddFile("Enemy.prefab");
    Q_UNUSED(enemyPrefabFile);
    auto* scriptsFolder = assetsFolder->AddFolder("Scripts");
    const auto* playerControllerFile = scriptsFolder->AddFile("PlayerController.cs");
    Q_UNUSED(playerControllerFile);
    const auto* gameManagerFile = scriptsFolder->AddFile("GameManager.cs");
    Q_UNUSED(gameManagerFile);
    explorerLayout->addWidget(explorerWidget);

    bottomUtilityPanel_ = new QWidget(centralWidget);
    bottomUtilityPanel_->setObjectName("BottomUtilityPanel");
    bottomUtilityPanel_->setFixedHeight(36);
    auto* bottomUtilityLayout = new QHBoxLayout(bottomUtilityPanel_);
    bottomUtilityLayout->setContentsMargins(8, 4, 8, 4);
    bottomUtilityLayout->setSpacing(8);

    openExplorerButton_ = new QPushButton("Open Explorer", bottomUtilityPanel_);
    bottomUtilityLayout->addWidget(openExplorerButton_);
    bottomUtilityLayout->addStretch(1);

    connect(openExplorerButton_, &QPushButton::clicked, this, [this]() {
        const bool shouldShowExplorer = explorerPanel_ != nullptr && !explorerPanel_->isVisible();
        if (explorerPanel_ != nullptr) {
            explorerPanel_->setVisible(shouldShowExplorer);
        }
        if (openExplorerButton_ != nullptr) {
            openExplorerButton_->setText(
                shouldShowExplorer ? "Close Explorer" : "Open Explorer"
            );
        }
    });

    rootLayout->addWidget(editorTitleBar_);
    rootLayout->addWidget(mainDockingWindow_, 1);
    rootLayout->addWidget(explorerPanel_);
    rootLayout->addWidget(bottomUtilityPanel_);

    setCentralWidget(centralWidget);

#ifdef Q_OS_WIN
    EnableSnapAndResize(this);
#endif

    const auto& config = editorApplication_.GetConfig();
    Q_UNUSED(config);
}

#ifdef Q_OS_WIN
bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
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
        } else if (editorTitleBar_ != nullptr) {
            const QPoint titleBarLocalPosition = editorTitleBar_->mapFromGlobal(QPoint(x, y));
            if (editorTitleBar_->rect().contains(titleBarLocalPosition)
                && editorTitleBar_->IsDragZone(titleBarLocalPosition)) {
                *result = HTCAPTION;
            } else {
                *result = HTCLIENT;
            }
        } else {
            *result = HTCLIENT;
        }

        return true;
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif

} // namespace Mirage::EditorQt
