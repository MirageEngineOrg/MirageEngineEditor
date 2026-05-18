#include "MainWindow/MainWindow.hpp"

#include "MainWindow/MainWindowInternal.hpp"

#include "Application/EditorApplication.hpp"

#include <QByteArray>
#include <QPoint>
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
    rootLayout->addWidget(editorTitleBar_);
    rootLayout->addStretch(1);

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
