#pragma once

#include "Application/EditorApplication.hpp"
#include "Docking/DockingTabBar.hpp"
#include "Docking/MainDockingWindow.hpp"
#include "EditorTitleBar/EditorTitleBar.hpp"

#include <QMainWindow>

class QPushButton;
class QWidget;

namespace Mirage::EditorQt {

class DockingWorkspacePage;

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif

private:
    Mirage::EditorCore::EditorApplication editorApplication_;
    EditorTitleBar* editorTitleBar_ {nullptr};
    DockingTabBar* mainDockingTabBar_ {nullptr};
    MainDockingWindow* mainDockingWindow_ {nullptr};
    QWidget* bottomUtilityPanel_ {nullptr};
    QWidget* explorerPanel_ {nullptr};
    QPushButton* openExplorerButton_ {nullptr};
};

} // namespace Mirage::EditorQt
