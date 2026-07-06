#pragma once

namespace Mirage::EditorQt {

class DockingTabBar;
class DockingWidget;

class DockingTabHost {
public:
    virtual ~DockingTabHost() = default;

    virtual int InsertDockWidgetIntoTabBar(
        DockingTabBar* targetTabBar,
        DockingWidget* dockingWidget,
        int index
    ) = 0;
};

} // namespace Mirage::EditorQt
