#include "Docking/DockingTabBar.hpp"

#include "Docking/DockingInternal.hpp"
#include "Docking/DockingTab.hpp"
#include "Docking/DockingWorkspacePage.hpp"
#include "Docking/DockingWindow.hpp"

#include <QApplication>
#include <QEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMouseEvent>
#include <QPixmap>
#include <QWindow>

namespace Mirage::EditorQt {

DockingTabBar::DockingTabBar(QWidget* parent)
    : QWidget(parent) {
    setObjectName("DockingTabBarSurface");
    setFixedHeight(30);
    installEventFilter(this);

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(11, 3, 11, 3);
    layout_->setSpacing(4);
    layout_->addStretch(1);

    ApplyStyles();
}

int DockingTabBar::AddTab(const QString& title, bool closable) {
    return InsertTab(static_cast<int>(tabs_.size()), title, closable);
}

int DockingTabBar::InsertTab(int index, const QString& title, bool closable) {
    auto* tab = new DockingTab(title, closable, this);

    const int boundedIndex = qBound(0, index, static_cast<int>(tabs_.size()));
    tabs_.insert(boundedIndex, tab);
    layout_->insertWidget(boundedIndex, tab);
    tab->installEventFilter(this);

    connect(tab, &DockingTab::Activated, this, [this, tab]() {
        const qsizetype indexOfTab = tabs_.indexOf(tab);
        if (indexOfTab >= 0) {
            SetCurrentIndex(static_cast<int>(indexOfTab));
        }
    });

    connect(tab, &DockingTab::CloseRequested, this, [this, tab]() {
        const qsizetype indexOfTab = tabs_.indexOf(tab);
        if (indexOfTab >= 0) {
            emit TabCloseRequested(static_cast<int>(indexOfTab));
        }
    });

    if (currentIndex_ < 0) {
        currentIndex_ = 0;
    } else if (boundedIndex <= currentIndex_) {
        ++currentIndex_;
    }

    RefreshTabStates();
    return boundedIndex;
}

void DockingTabBar::ContinueTransferredDrag(
    int index,
    const QPoint& globalPosition,
    const QPoint& grabOffset
) {
    if (index < 0 || index >= tabs_.size()) {
        return;
    }

    dragTab_ = tabs_.at(index);
    dragStartGlobalPosition_ = globalPosition;

    if (dragInProgress_ || dragTab_ == nullptr) {
        return;
    }

    dragOriginalIndex_ = index;
    dragInProgress_ = true;
    dragTargetIndex_ = dragOriginalIndex_;
    dragGrabOffset_ = grabOffset;

    dragPlaceholder_ = new QWidget(this);
    dragPlaceholder_->setFixedSize(dragTab_->size());
    dragPlaceholder_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dragPlaceholder_->setObjectName("DockingTabPlaceholder");

    EnsureDragPreview();

    layout_->removeWidget(dragTab_);
    layout_->insertWidget(dragOriginalIndex_, dragPlaceholder_);

    dragTab_->hide();
    dragTab_->grabMouse();
    MovePreview(globalPosition);
    dragPreview_->show();
}

int DockingTabBar::GetCurrentIndex() const noexcept {
    return currentIndex_;
}

void DockingTabBar::SetHost(DockingTabHost* host) noexcept {
    host_ = host;
}

int DockingTabBar::GetTabCount() const noexcept {
    return static_cast<int>(tabs_.size());
}

DockingTabHost* DockingTabBar::GetHost() const noexcept {
    return host_;
}

int DockingTabBar::GetExternalPlaceholderIndex() const noexcept {
    return externalPlaceholderIndex_;
}

void DockingTabBar::MoveTab(const int from_idx, const int to_idx) {
    if (from_idx < 0 || from_idx >= tabs_.size() || to_idx < 0 || to_idx >= tabs_.size() || from_idx == to_idx) {
        return;
    }

    DockingTab* tab = tabs_.takeAt(from_idx);
    tabs_.insert(to_idx, tab);
    layout_->removeWidget(tab);
    layout_->insertWidget(to_idx, tab);

    if (currentIndex_ == from_idx) {
        currentIndex_ = to_idx;
    } else if (from_idx < currentIndex_ && to_idx >= currentIndex_) {
        --currentIndex_;
    } else if (from_idx > currentIndex_ && to_idx <= currentIndex_) {
        ++currentIndex_;
    }

    RefreshTabStates();
    emit CurrentChanged(currentIndex_);
}

bool DockingTabBar::IsDragZone(const QPoint& localPosition) const {
    QWidget* child = childAt(localPosition);
    if (child == nullptr) {
        return true;
    }
    if (auto* tab = qobject_cast<DockingTab*>(child); tab != nullptr) {
        return tab->IsDragZone(tab->mapFrom(this, localPosition));
    }

    return false;
}

void DockingTabBar::ApplyStyles() {
    QFile styleSheetFile(QString::fromUtf8(kDockingStyleSheetPath));
    if (!styleSheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    setStyleSheet(QString::fromUtf8(styleSheetFile.readAll()));
}

void DockingTabBar::BeginTabDrag(DockingTab* tab, const QPoint& globalPosition) {
    if (dragInProgress_ || tab == nullptr) {
        return;
    }

    dragOriginalIndex_ = static_cast<int>(tabs_.indexOf(tab));
    if (dragOriginalIndex_ < 0) {
        ResetDragState();
        return;
    }

    dragInProgress_ = true;
    dragTargetIndex_ = dragOriginalIndex_;
    dragGrabOffset_ = tab->mapFromGlobal(globalPosition);

    dragPlaceholder_ = new QWidget(this);
    dragPlaceholder_->setFixedSize(tab->size());
    dragPlaceholder_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dragPlaceholder_->setObjectName("DockingTabPlaceholder");

    EnsureDragPreview();

    layout_->removeWidget(tab);
    layout_->insertWidget(dragOriginalIndex_, dragPlaceholder_);

    tab->hide();
    tab->grabMouse();
    MovePreview(globalPosition);
    dragPreview_->show();
}

void DockingTabBar::BeginWindowDrag(DockingTab* tab, const QPoint& globalPosition) {
    if (dragInProgress_ || tab == nullptr) {
        return;
    }

    auto* dockingWindow = dynamic_cast<DockingWindow*>(parentWidget());
    if (dockingWindow == nullptr || !dockingWindow->IsFloatingWindow()) {
        return;
    }

    dragOriginalIndex_ = static_cast<int>(tabs_.indexOf(tab));
    if (dragOriginalIndex_ < 0) {
        ResetDragState();
        return;
    }

    dragInProgress_ = true;
    windowDragInProgress_ = true;
    dragTargetIndex_ = dragOriginalIndex_;
    dragGrabOffset_ = tab->mapFromGlobal(globalPosition);
    windowDragOffset_ = dockingWindow->mapFromGlobal(globalPosition);
    tab->grabMouse();
}

void DockingTabBar::CommitDragMove() {
    if (!dragInProgress_ || dragTab_ == nullptr) {
        ResetDragState();
        return;
    }

    const int from_idx = dragOriginalIndex_;
    const int to_idx = dragTargetIndex_;

    if (dragPlaceholder_ != nullptr) {
        layout_->removeWidget(dragPlaceholder_);
        delete dragPlaceholder_;
        dragPlaceholder_ = nullptr;
    }

    layout_->insertWidget(to_idx, dragTab_);
    dragTab_->releaseMouse();
    dragTab_->show();
    dragTab_->move(0, 0);

    if (from_idx >= 0 && to_idx >= 0 && from_idx != to_idx) {
        DockingTab* movedTab = tabs_.takeAt(from_idx);
        tabs_.insert(to_idx, movedTab);

        if (currentIndex_ == from_idx) {
            currentIndex_ = to_idx;
        } else if (from_idx < currentIndex_ && to_idx >= currentIndex_) {
            --currentIndex_;
        } else if (from_idx > currentIndex_ && to_idx <= currentIndex_) {
            ++currentIndex_;
        }

        emit TabMoveRequested(from_idx, to_idx);
    }

    RefreshTabStates();
    emit CurrentChanged(currentIndex_);
    ResetDragState();
}

void DockingTabBar::CommitDragTransfer(DockingTabBar* targetTabBar) {
    if (!dragInProgress_ || dragTab_ == nullptr || targetTabBar == nullptr) {
        ResetDragState();
        return;
    }

    const int from = dragOriginalIndex_;
    const int to = targetTabBar->GetExternalPlaceholderIndex();

    dragTab_->releaseMouse();
    dragTab_->show();

    emit TabTransferRequested(from, targetTabBar, to >= 0 ? to : targetTabBar->GetTabCount());
    ResetDragState();
}

bool DockingTabBar::CommitWorkspaceDrop(
    DockingWorkspacePage* targetWorkspacePage,
    const QPoint& globalPosition
) {
    if (!dragInProgress_ || dragTab_ == nullptr || targetWorkspacePage == nullptr) {
        ResetDragState();
        return false;
    }

    dragTab_->releaseMouse();
    dragTab_->show();

    auto* dockingWindow = dynamic_cast<DockingWindow*>(parentWidget());
    const bool transferred = dockingWindow != nullptr
        && dockingWindow->TransferDockWidgetToWorkspace(
            dragOriginalIndex_,
            targetWorkspacePage,
            globalPosition
        );
    ResetDragState();
    return transferred;
}

int DockingTabBar::ComputeDropIndex(int localX) const {
    int targetIndex = 0;
    for (const auto *const tab : tabs_) {
        if (tab == dragTab_) {
            continue;
        }

        if (localX > tab->geometry().center().x()) {
            ++targetIndex;
        }
    }

    return qBound(0, targetIndex, static_cast<int>(tabs_.size() - 1));
}

int DockingTabBar::ComputeDropIndexForExternalPosition(int localX) const {
    int targetIndex = 0;
    for (const auto *const tab : tabs_) {
        if (localX > tab->geometry().center().x()) {
            ++targetIndex;
        }
    }

    return qBound(0, targetIndex, static_cast<int>(tabs_.size()));
}

DockingTabBar* DockingTabBar::FindDockingTabBarAtGlobal(const QPoint& globalPosition) const {
    DockingTabBar* selfCandidate = nullptr;
    DockingTabBar* foreignCandidate = nullptr;

    const auto widgets = QApplication::allWidgets();
    for (QWidget* widget : widgets) {
        auto* tabBar = qobject_cast<DockingTabBar*>(widget);
        if (tabBar == nullptr || !tabBar->isVisible()) {
            continue;
        }

        if (tabBar->GetHost() == nullptr) {
            continue;
        }

        const QPoint topLeft = tabBar->mapToGlobal(QPoint(0, 0));
        const QRect globalRect(topLeft, tabBar->size());
        if (globalRect.contains(globalPosition)) {
            if (tabBar == this) {
                selfCandidate = tabBar;
            } else if (foreignCandidate == nullptr) {
                foreignCandidate = tabBar;
            }
        }
    }

    if (windowDragInProgress_ && foreignCandidate != nullptr) {
        return foreignCandidate;
    }

    if (selfCandidate != nullptr) {
        return selfCandidate;
    }

    return foreignCandidate;
}

DockingWorkspacePage* DockingTabBar::FindWorkspacePageAtGlobal(const QPoint& globalPosition) const {
    const auto widgets = QApplication::allWidgets();
    for (QWidget* widget : widgets) {
        auto* workspacePage = dynamic_cast<DockingWorkspacePage*>(widget);
        if (workspacePage == nullptr || !workspacePage->ContainsGlobalPosition(globalPosition)) {
            continue;
        }

        return workspacePage;
    }

    return nullptr;
}

void DockingTabBar::EnsureDragPreview() {
    if (dragPreview_ != nullptr || dragTab_ == nullptr) {
        return;
    }

    dragPreview_ = new QLabel(nullptr);
    dragPreview_->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    dragPreview_->setAttribute(Qt::WA_TransparentForMouseEvents);
    dragPreview_->setPixmap(dragTab_->grab());
    dragPreview_->setFixedSize(dragTab_->size());
}

void DockingTabBar::MovePlaceholder(int targetIndex) {
    if (dragPlaceholder_ == nullptr || targetIndex == dragTargetIndex_) {
        return;
    }

    dragTargetIndex_ = targetIndex;
    layout_->removeWidget(dragPlaceholder_);
    layout_->insertWidget(dragTargetIndex_, dragPlaceholder_);
}

void DockingTabBar::MovePreview(const QPoint& globalPosition) {
    if (dragPreview_ == nullptr) {
        return;
    }

    dragPreview_->move(globalPosition - dragGrabOffset_);
}

void DockingTabBar::MoveWindowDrag(const QPoint& globalPosition) {
    auto* dockingWindow = dynamic_cast<DockingWindow*>(parentWidget());
    if (dockingWindow == nullptr || !dockingWindow->IsFloatingWindow()) {
        return;
    }

    dockingWindow->move(globalPosition - windowDragOffset_);
}

void DockingTabBar::ShowExternalPlaceholder(int index) {
    const int boundedIndex = qBound(0, index, static_cast<int>(tabs_.size()));
    if (externalPlaceholder_ == nullptr) {
        externalPlaceholder_ = new QWidget(this);
        externalPlaceholder_->setFixedSize(
            dragTab_ != nullptr ? dragTab_->size() : QSize(120, 24)
        );
        externalPlaceholder_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        externalPlaceholder_->setObjectName("DockingTabPlaceholder");
    }

    externalPlaceholderIndex_ = boundedIndex;
    layout_->removeWidget(externalPlaceholder_);
    layout_->insertWidget(externalPlaceholderIndex_, externalPlaceholder_);
}

void DockingTabBar::ClearExternalPlaceholder() {
    if (externalPlaceholder_ != nullptr) {
        layout_->removeWidget(externalPlaceholder_);
        delete externalPlaceholder_;
        externalPlaceholder_ = nullptr;
    }

    externalPlaceholderIndex_ = -1;
}

void DockingTabBar::ResetDragStateWithoutRestore() {
    if (dragTab_ != nullptr) {
        dragTab_->releaseMouse();
    }

    if (dragPreview_ != nullptr) {
        dragPreview_->hide();
        delete dragPreview_;
    }

    if (dragPlaceholder_ != nullptr) {
        layout_->removeWidget(dragPlaceholder_);
        delete dragPlaceholder_;
    }

    if (hoverTargetTabBar_ != nullptr && hoverTargetTabBar_ != this) {
        hoverTargetTabBar_->ClearExternalPlaceholder();
    }
    if (hoverWorkspacePage_ != nullptr) {
        hoverWorkspacePage_->ClearDropPlaceholder();
    }

    dragPreview_ = nullptr;
    dragPlaceholder_ = nullptr;
    dragTab_ = nullptr;
    dragInProgress_ = false;
    windowDragInProgress_ = false;
    hoverTargetTabBar_ = nullptr;
    hoverWorkspacePage_ = nullptr;
    dragOriginalIndex_ = -1;
    dragTargetIndex_ = -1;
    externalPlaceholderIndex_ = -1;
    dragGrabOffset_ = {};
    dragStartGlobalPosition_ = {};
    windowDragOffset_ = {};
}

void DockingTabBar::ResetDragState() {
    if (dragTab_ != nullptr) {
        dragTab_->releaseMouse();
        dragTab_->show();
    }

    if (dragPreview_ != nullptr) {
        dragPreview_->hide();
        delete dragPreview_;
    }

    if (dragPlaceholder_ != nullptr) {
        layout_->removeWidget(dragPlaceholder_);
        delete dragPlaceholder_;
    }

    if (hoverTargetTabBar_ != nullptr && hoverTargetTabBar_ != this) {
        hoverTargetTabBar_->ClearExternalPlaceholder();
    }
    if (hoverWorkspacePage_ != nullptr) {
        hoverWorkspacePage_->ClearDropPlaceholder();
    }

    dragPreview_ = nullptr;
    dragPlaceholder_ = nullptr;
    dragTab_ = nullptr;
    dragInProgress_ = false;
    windowDragInProgress_ = false;
    hoverTargetTabBar_ = nullptr;
    hoverWorkspacePage_ = nullptr;
    dragOriginalIndex_ = -1;
    dragTargetIndex_ = -1;
    externalPlaceholderIndex_ = -1;
    dragGrabOffset_ = {};
    dragStartGlobalPosition_ = {};
    windowDragOffset_ = {};
}

bool DockingTabBar::IsFloatingDragHandle(QObject* watched, const QPoint& localPosition) const {
    auto* dockingWindow = dynamic_cast<DockingWindow*>(parentWidget());
    if (dockingWindow == nullptr || !dockingWindow->IsFloatingWindow()) {
        return false;
    }

    if (watched == this) {
        return childAt(localPosition) == nullptr;
    }

    auto* tab = qobject_cast<DockingTab*>(watched);
    return tab != nullptr && tab->IsDragZone(localPosition);
}

void DockingTabBar::RemoveTab(int index) {
    if (index < 0 || index >= tabs_.size()) {
        return;
    }

    DockingTab* tab = tabs_.takeAt(index);
    layout_->removeWidget(tab);
    tab->deleteLater();

    if (tabs_.isEmpty()) {
        currentIndex_ = -1;
    } else if (currentIndex_ >= static_cast<int>(tabs_.size())) {
        currentIndex_ = static_cast<int>(tabs_.size() - 1);
    } else if (index <= currentIndex_) {
        currentIndex_ = qMax(0, currentIndex_ - 1);
    }

    RefreshTabStates();
    emit CurrentChanged(currentIndex_);
}

void DockingTabBar::SetCurrentIndex(int index) {
    if (index < 0 || index >= tabs_.size() || currentIndex_ == index) {
        return;
    }

    currentIndex_ = index;
    RefreshTabStates();
    emit CurrentChanged(currentIndex_);
}

void DockingTabBar::RefreshTabStates() {
    for (qsizetype index = 0; index < tabs_.size(); ++index) {
        tabs_.at(index)->SetActive(static_cast<int>(index) == currentIndex_);
    }
}

bool DockingTabBar::eventFilter(QObject* watched, QEvent* event) {
    if (auto* tab = qobject_cast<DockingTab*>(watched); tab != nullptr) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton
                && tab->IsDragZone(mouseEvent->position().toPoint())) {
                dragTab_ = tab;
                dragStartGlobalPosition_ = mouseEvent->globalPosition().toPoint();
            }
        } else if (event->type() == QEvent::MouseMove && dragTab_ == tab) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            const QPoint currentGlobalPosition = mouseEvent->globalPosition().toPoint();
            if ((currentGlobalPosition - dragStartGlobalPosition_).manhattanLength()
                < QApplication::startDragDistance()) {
                return QWidget::eventFilter(watched, event);
            }

            if (!dragInProgress_) {
                if (tabs_.size() <= 1) {
                    auto* dockingWindow = dynamic_cast<DockingWindow*>(parentWidget());
                    if (dockingWindow != nullptr && dockingWindow->IsEmbeddedInWorkspace()) {
                        if (dockingWindow->DetachFromWorkspaceToFloating(currentGlobalPosition)) {
                            BeginWindowDrag(tab, currentGlobalPosition);
                        } else {
                            BeginTabDrag(tab, currentGlobalPosition);
                        }
                    } else if (dockingWindow != nullptr && dockingWindow->IsFloatingWindow()) {
                        BeginWindowDrag(tab, currentGlobalPosition);
                    } else {
                        BeginTabDrag(tab, currentGlobalPosition);
                    }
                } else {
                    BeginTabDrag(tab, currentGlobalPosition);
                }
            }

            if (!dragInProgress_) {
                ResetDragState();
                return QWidget::eventFilter(watched, event);
            }

            DockingTabBar* targetTabBar = FindDockingTabBarAtGlobal(currentGlobalPosition);
            DockingWorkspacePage* targetWorkspacePage =
                targetTabBar == nullptr ? FindWorkspacePageAtGlobal(currentGlobalPosition) : nullptr;
            if (hoverTargetTabBar_ != nullptr && hoverTargetTabBar_ != this
                && hoverTargetTabBar_ != targetTabBar) {
                hoverTargetTabBar_->ClearExternalPlaceholder();
            }
            if (hoverWorkspacePage_ != nullptr && hoverWorkspacePage_ != targetWorkspacePage) {
                hoverWorkspacePage_->ClearDropPlaceholder();
            }
            hoverTargetTabBar_ = targetTabBar;
            hoverWorkspacePage_ = targetWorkspacePage;

            if (windowDragInProgress_) {
                if (targetTabBar != nullptr && targetTabBar != this) {
                    const int targetIndex = targetTabBar->ComputeDropIndexForExternalPosition(
                        targetTabBar->mapFromGlobal(currentGlobalPosition).x()
                    );
                    auto* dockingWindow = dynamic_cast<DockingWindow*>(parentWidget());
                    const int sourceIndex = dragOriginalIndex_;
                    const QPoint sourceGrabOffset = dragGrabOffset_;
                    ResetDragStateWithoutRestore();
                    if (dockingWindow != nullptr) {
                        dockingWindow->TransferDockWidgetTo(
                            sourceIndex,
                            targetTabBar,
                            targetIndex,
                            currentGlobalPosition,
                            sourceGrabOffset,
                            true
                        );
                    }
                    return true;
                }

                MoveWindowDrag(currentGlobalPosition);
            } else {
                MovePreview(currentGlobalPosition);
            }

            if (targetTabBar != nullptr) {
                const QPoint targetLocalPosition = targetTabBar->mapFromGlobal(currentGlobalPosition);
                if (targetTabBar == this) {
                    if (windowDragInProgress_) {
                        ClearExternalPlaceholder();
                        return true;
                    }

                    if (targetLocalPosition.y() < 0 || targetLocalPosition.y() > height()) {
                        if (hoverTargetTabBar_ != nullptr && hoverTargetTabBar_ != this) {
                            hoverTargetTabBar_->ClearExternalPlaceholder();
                        }
                    } else {
                        MovePlaceholder(ComputeDropIndex(targetLocalPosition.x()));
                    }
                } else {
                    targetTabBar->ShowExternalPlaceholder(
                        targetTabBar->ComputeDropIndexForExternalPosition(targetLocalPosition.x())
                    );
                }
            } else if (targetWorkspacePage != nullptr) {
                targetWorkspacePage->ShowDropPlaceholder(currentGlobalPosition);
            }

            return true;
        } else if (event->type() == QEvent::MouseButtonRelease && dragTab_ == tab) {
            if (dragInProgress_) {
                const QPoint releaseGlobalPosition =
                    static_cast<QMouseEvent*>(event)->globalPosition().toPoint();
                DockingTabBar* targetTabBar = FindDockingTabBarAtGlobal(releaseGlobalPosition);
                DockingWorkspacePage* targetWorkspacePage =
                    targetTabBar == nullptr ? FindWorkspacePageAtGlobal(releaseGlobalPosition) : nullptr;
                if (targetTabBar != nullptr && targetTabBar != this) {
                    CommitDragTransfer(targetTabBar);
                } else if (targetWorkspacePage != nullptr) {
                    CommitWorkspaceDrop(targetWorkspacePage, releaseGlobalPosition);
                } else if (targetTabBar == this && !windowDragInProgress_) {
                    CommitDragMove();
                } else {
                    if (!windowDragInProgress_) {
                        emit TabDetachRequested(dragOriginalIndex_, releaseGlobalPosition);
                    }
                    ResetDragState();
                }
                return true;
            }

            ResetDragState();
        }
    }

#ifdef Q_OS_WIN
    Q_UNUSED(watched);
    return QWidget::eventFilter(watched, event);
#else
    if (event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton
            && IsFloatingDragHandle(watched, mouseEvent->position().toPoint())) {
            if (QWidget* topLevelWidget = window(); topLevelWidget != nullptr) {
                if (QWindow* nativeWindow = topLevelWidget->windowHandle();
                    nativeWindow != nullptr) {
                    nativeWindow->startSystemMove();
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(watched, event);
#endif
}

} // namespace Mirage::EditorQt
