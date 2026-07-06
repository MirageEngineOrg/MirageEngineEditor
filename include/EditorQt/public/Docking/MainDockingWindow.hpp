#pragma once

#include "Docking/DockingWindow.hpp"

namespace Mirage::EditorQt {

class DockingTabBar;

class MainDockingWindow final : public DockingWindow {
public:
    explicit MainDockingWindow(DockingTabBar* externalTabBar, QWidget* parent = nullptr);
};

} // namespace Mirage::EditorQt
