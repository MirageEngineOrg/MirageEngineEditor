#pragma once

#include <QWidget>

class QEvent;
class QHBoxLayout;
class QLabel;
class QObject;
class QPoint;
class QString;

namespace Mirage::EditorQt {

class DockingTab;

class DockingTabBar final : public QWidget {
    Q_OBJECT

public:
    explicit DockingTabBar(QWidget* parent = nullptr);

    [[nodiscard]] int AddTab(const QString& title, bool closable);
    [[nodiscard]] int InsertTab(int index, const QString& title, bool closable);
    void ContinueTransferredDrag(
        int index,
        const QPoint& globalPosition,
        const QPoint& grabOffset
    );
    [[nodiscard]] int GetCurrentIndex() const noexcept;
    [[nodiscard]] int GetTabCount() const noexcept;
    [[nodiscard]] bool IsDragZone(const QPoint& localPosition) const;
    [[nodiscard]] bool IsFloatingDragHandle(QObject* watched, const QPoint& localPosition) const;
    [[nodiscard]] int GetExternalPlaceholderIndex() const noexcept;
    void MoveTab(int from_idx, int to_idx);
    void RemoveTab(int index);
    void SetCurrentIndex(int index);
    void ClearExternalPlaceholder();
    void ShowExternalPlaceholder(int index);

signals:
    void CurrentChanged(int index);
    void TabDetachRequested(int index, QPoint globalPosition);
    void TabCloseRequested(int index);
    void TabMoveRequested(int from_idx, int to_idx);
    void TabTransferRequested(int from, Mirage::EditorQt::DockingTabBar* targetTabBar, int to_idx);
private:
    void ApplyStyles();
    void BeginTabDrag(DockingTab* tab, const QPoint& globalPosition);
    void BeginWindowDrag(DockingTab* tab, const QPoint& globalPosition);
    void CommitDragMove();
    void CommitDragTransfer(DockingTabBar* targetTabBar);
    [[nodiscard]] int ComputeDropIndex(int localX) const;
    [[nodiscard]] int ComputeDropIndexForExternalPosition(int localX) const;
    [[nodiscard]] DockingTabBar* FindDockingTabBarAtGlobal(const QPoint& globalPosition) const;
    void EnsureDragPreview();
    void MovePlaceholder(int targetIndex);
    void MovePreview(const QPoint& globalPosition);
    void MoveWindowDrag(const QPoint& globalPosition);
    void ResetDragStateWithoutRestore();
    void ResetDragState();
    bool eventFilter(QObject* watched, QEvent* event) override;
    void RefreshTabStates();

    QWidget* dragPlaceholder_ {nullptr};
    QWidget* externalPlaceholder_ {nullptr};
    DockingTab* dragTab_ {nullptr};
    QLabel* dragPreview_ {nullptr};
    bool dragInProgress_ {false};
    bool windowDragInProgress_ {false};
    DockingTabBar* hoverTargetTabBar_ {nullptr};
    int dragOriginalIndex_ {-1};
    int dragTargetIndex_ {-1};
    int externalPlaceholderIndex_ {-1};
    QHBoxLayout* layout_ {nullptr};
    QList<DockingTab*> tabs_;
    int currentIndex_ {-1};
    QPoint dragGrabOffset_;
    QPoint dragStartGlobalPosition_;
    QPoint windowDragOffset_;
};

} // namespace Mirage::EditorQt
