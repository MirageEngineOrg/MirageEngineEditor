#pragma once

#include "Docking/DockingWidget.hpp"

class QSplitter;
class QWidget;

namespace Mirage::EditorQt {

class DockingWindow;

class DockingWorkspacePage final : public DockingWidget {
public:
    explicit DockingWorkspacePage(
        QString title,
        bool closable,
        Qt::Orientation orientation,
        QWidget* parent = nullptr
    );

    void AddDockWindow(DockingWindow* dockingWindow);
    void RemoveDockWindow(DockingWindow* dockingWindow);
    void ShowDropPlaceholder(const QPoint& globalPosition);
    void ClearDropPlaceholder();
    [[nodiscard]] bool ContainsGlobalPosition(const QPoint& globalPosition) const;
    [[nodiscard]] bool InsertDockWidgetAtDropPlaceholder(
        DockingWidget* dockingWidget,
        const QPoint& globalPosition
    );

private:
    enum class DropSide {
        None,
        Left,
        Right,
        Top,
        Bottom
    };

    [[nodiscard]] DockingWindow* FindDockingWindowAtGlobal(const QPoint& globalPosition) const;
    [[nodiscard]] QSplitter* FindParentSplitterFor(QWidget* widget) const;
    [[nodiscard]] DropSide ComputeDropSide(
        const DockingWindow* dockingWindow,
        const QPoint& globalPosition
    ) const;
    [[nodiscard]] QRect ComputeDropPlaceholderGeometry(
        const DockingWindow* dockingWindow,
        DropSide side
    ) const;
    [[nodiscard]] bool InsertDockWindowBesideTarget(
        DockingWindow* targetDockWindow,
        DockingWindow* newDockWindow,
        DropSide side
    );
    void CleanupSplitterTree(QSplitter* splitter);

    QSplitter* rootSplitter_ {nullptr};
    QWidget* dropPlaceholder_ {nullptr};
    DockingWindow* dropTargetDockWindow_ {nullptr};
    DropSide dropSide_ {DropSide::None};
};

} // namespace Mirage::EditorQt
