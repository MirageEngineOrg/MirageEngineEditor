#include "Docking/MainDockingWindow.hpp"

#include "Docking/DockingTabBar.hpp"

namespace Mirage::EditorQt {

MainDockingWindow::MainDockingWindow(DockingTabBar* externalTabBar, QWidget* parent)
    : DockingWindow(externalTabBar, parent) {
    setObjectName("MainDockingWindow");
    SetTabBarVisible(false);
}

} // namespace Mirage::EditorQt
