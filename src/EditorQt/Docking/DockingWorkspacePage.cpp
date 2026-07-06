#include "Docking/DockingWorkspacePage.hpp"

#include "Docking/DockingWidget.hpp"
#include "Docking/DockingWindow.hpp"

#include <QApplication>
#include <QFrame>
#include <QRect>
#include <QSplitter>
#include <QVBoxLayout>

namespace Mirage::EditorQt {

DockingWorkspacePage::DockingWorkspacePage(
    QString title,
    const bool closable,
    Qt::Orientation orientation,
    QWidget* parent
)
    : DockingWidget(std::move(title), closable, parent) {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    rootSplitter_ = new QSplitter(orientation, this);
    rootSplitter_->setChildrenCollapsible(false);

    rootLayout->addWidget(rootSplitter_);
}

void DockingWorkspacePage::AddDockWindow(DockingWindow* dockingWindow) {
    if (dockingWindow == nullptr) {
        return;
    }

    dockingWindow->setParent(rootSplitter_);
    rootSplitter_->addWidget(dockingWindow);
}

void DockingWorkspacePage::RemoveDockWindow(DockingWindow* dockingWindow) {
    if (dockingWindow == nullptr) {
        return;
    }

    QSplitter* parentSplitter = FindParentSplitterFor(dockingWindow);
    if (parentSplitter == nullptr) {
        return;
    }

    dockingWindow->setParent(nullptr);
    CleanupSplitterTree(parentSplitter);
}

void DockingWorkspacePage::ShowDropPlaceholder(const QPoint& globalPosition) {
    if (dropPlaceholder_ == nullptr) {
        auto* placeholderFrame = new QFrame(this);
        placeholderFrame->setObjectName("DockingWorkspaceDropPlaceholder");
        placeholderFrame->setStyleSheet(
            "background-color: rgba(64, 156, 255, 160);"
            "border: 2px solid rgb(64, 156, 255);"
            "border-radius: 6px;"
        );
        dropPlaceholder_ = placeholderFrame;
    }

    DockingWindow* targetDockWindow = FindDockingWindowAtGlobal(globalPosition);
    if (targetDockWindow == nullptr) {
        dropPlaceholder_->hide();
        dropTargetDockWindow_ = nullptr;
        dropSide_ = DropSide::None;
        return;
    }

    const DropSide side = ComputeDropSide(targetDockWindow, globalPosition);
    if (side == DropSide::None) {
        dropPlaceholder_->hide();
        dropTargetDockWindow_ = nullptr;
        dropSide_ = DropSide::None;
        return;
    }

    dropTargetDockWindow_ = targetDockWindow;
    dropSide_ = side;
    dropPlaceholder_->setGeometry(ComputeDropPlaceholderGeometry(targetDockWindow, side));
    dropPlaceholder_->raise();
    dropPlaceholder_->show();
}

void DockingWorkspacePage::ClearDropPlaceholder() {
    if (dropPlaceholder_ != nullptr) {
        dropPlaceholder_->hide();
    }

    dropTargetDockWindow_ = nullptr;
    dropSide_ = DropSide::None;
}

bool DockingWorkspacePage::ContainsGlobalPosition(const QPoint& globalPosition) const {
    if (!isVisible()) {
        return false;
    }

    const QRect globalRect(mapToGlobal(QPoint(0, 0)), size());
    return globalRect.contains(globalPosition);
}

bool DockingWorkspacePage::InsertDockWidgetAtDropPlaceholder(
    DockingWidget* dockingWidget,
    const QPoint& globalPosition
) {
    if (dockingWidget == nullptr) {
        return false;
    }

    ShowDropPlaceholder(globalPosition);
    if (dropTargetDockWindow_ == nullptr || dropSide_ == DropSide::None) {
        return false;
    }

    auto* dockingWindow = new DockingWindow(this);
    dockingWindow->addDockWidget(dockingWidget);
    if (!InsertDockWindowBesideTarget(dropTargetDockWindow_, dockingWindow, dropSide_)) {
        dockingWindow->deleteLater();
        return false;
    }

    ClearDropPlaceholder();
    return true;
}

DockingWindow* DockingWorkspacePage::FindDockingWindowAtGlobal(const QPoint& globalPosition) const {
    DockingWindow* bestCandidate = nullptr;
    int bestArea = std::numeric_limits<int>::max();

    const auto widgets = QApplication::allWidgets();
    for (QWidget* widget : widgets) {
        auto* dockingWindow = dynamic_cast<DockingWindow*>(widget);
        if (dockingWindow == nullptr || dockingWindow->IsFloatingWindow() || !isAncestorOf(dockingWindow)
            || !dockingWindow->isVisible()) {
            continue;
        }

        const QRect globalRect(
            dockingWindow->mapToGlobal(QPoint(0, 0)),
            dockingWindow->size()
        );
        if (!globalRect.contains(globalPosition)) {
            continue;
        }

        const int area = globalRect.width() * globalRect.height();
        if (area < bestArea) {
            bestArea = area;
            bestCandidate = dockingWindow;
        }
    }

    return bestCandidate;
}

QSplitter* DockingWorkspacePage::FindParentSplitterFor(QWidget* widget) const {
    return qobject_cast<QSplitter*>(widget != nullptr ? widget->parentWidget() : nullptr);
}

DockingWorkspacePage::DropSide DockingWorkspacePage::ComputeDropSide(
    const DockingWindow* dockingWindow,
    const QPoint& globalPosition
) const {
    if (dockingWindow == nullptr) {
        return DropSide::None;
    }

    const QPoint localPosition = dockingWindow->mapFromGlobal(globalPosition);
    const int leftDistance = std::abs(localPosition.x());
    const int rightDistance = std::abs(dockingWindow->width() - localPosition.x());
    const int topDistance = std::abs(localPosition.y());
    const int bottomDistance = std::abs(dockingWindow->height() - localPosition.y());

    int bestDistance = leftDistance;
    DropSide bestSide = DropSide::Left;

    if (rightDistance < bestDistance) {
        bestDistance = rightDistance;
        bestSide = DropSide::Right;
    }
    if (topDistance < bestDistance) {
        bestDistance = topDistance;
        bestSide = DropSide::Top;
    }
    if (bottomDistance < bestDistance) {
        bestDistance = bottomDistance;
        bestSide = DropSide::Bottom;
    }

    return bestSide;
}

QRect DockingWorkspacePage::ComputeDropPlaceholderGeometry(
    const DockingWindow* dockingWindow,
    const DropSide side
) const {
    constexpr int kInset = 8;

    QRect rect(
        mapFromGlobal(dockingWindow->mapToGlobal(QPoint(0, 0))),
        dockingWindow->size()
    );
    const int edgeWidth = qBound(28, rect.width() / 4, 96);
    const int edgeHeight = qBound(28, rect.height() / 4, 96);

    switch (side) {
    case DropSide::Left:
        rect.setWidth(edgeWidth);
        break;
    case DropSide::Right:
        rect.setX(rect.right() - edgeWidth + 1);
        rect.setWidth(edgeWidth);
        break;
    case DropSide::Top:
        rect.setHeight(edgeHeight);
        break;
    case DropSide::Bottom:
        rect.setY(rect.bottom() - edgeHeight + 1);
        rect.setHeight(edgeHeight);
        break;
    case DropSide::None:
        break;
    }

    return rect.adjusted(kInset, kInset, -kInset, -kInset);
}

bool DockingWorkspacePage::InsertDockWindowBesideTarget(
    DockingWindow* targetDockWindow,
    DockingWindow* newDockWindow,
    const DropSide side
) {
    if (targetDockWindow == nullptr || newDockWindow == nullptr || side == DropSide::None) {
        return false;
    }

    QSplitter* parentSplitter = FindParentSplitterFor(targetDockWindow);
    if (parentSplitter == nullptr) {
        return false;
    }

    const bool horizontal = side == DropSide::Left || side == DropSide::Right;
    const Qt::Orientation desiredOrientation =
        horizontal ? Qt::Horizontal : Qt::Vertical;
    const bool insertBefore = side == DropSide::Left || side == DropSide::Top;

    if (parentSplitter->orientation() == desiredOrientation) {
        const int targetIndex = parentSplitter->indexOf(targetDockWindow);
        parentSplitter->insertWidget(insertBefore ? targetIndex : targetIndex + 1, newDockWindow);
        return true;
    }

    const int targetIndex = parentSplitter->indexOf(targetDockWindow);
    if (targetIndex < 0) {
        return false;
    }

    auto* nestedSplitter = new QSplitter(desiredOrientation, parentSplitter);
    nestedSplitter->setChildrenCollapsible(false);

    targetDockWindow->setParent(nullptr);
    parentSplitter->insertWidget(targetIndex, nestedSplitter);

    if (insertBefore) {
        nestedSplitter->addWidget(newDockWindow);
        nestedSplitter->addWidget(targetDockWindow);
    } else {
        nestedSplitter->addWidget(targetDockWindow);
        nestedSplitter->addWidget(newDockWindow);
    }

    return true;
}

void DockingWorkspacePage::CleanupSplitterTree(QSplitter* splitter) {
    if (splitter == nullptr) {
        return;
    }

    if (splitter->count() > 1) {
        return;
    }

    if (splitter == rootSplitter_) {
        return;
    }

    QSplitter* parentSplitter = qobject_cast<QSplitter*>(splitter->parentWidget());
    if (parentSplitter == nullptr) {
        return;
    }

    if (splitter->count() == 0) {
        splitter->setParent(nullptr);
        splitter->deleteLater();
        CleanupSplitterTree(parentSplitter);
        return;
    }

    QWidget* onlyChild = splitter->widget(0);
    const int splitterIndex = parentSplitter->indexOf(splitter);
    if (onlyChild == nullptr || splitterIndex < 0) {
        return;
    }

    onlyChild->setParent(nullptr);
    splitter->setParent(nullptr);
    parentSplitter->insertWidget(splitterIndex, onlyChild);
    splitter->deleteLater();
    CleanupSplitterTree(parentSplitter);
}

} // namespace Mirage::EditorQt
