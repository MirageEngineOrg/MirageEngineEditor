#pragma once

#include <QWidget>
#include <QPointer>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QPoint;

namespace Mirage::EditorQt {

class ExplorerEntry;
class ExplorerFolderEntry;
class ExplorerItemWidget;

class ExplorerWidget final : public QWidget {
    Q_OBJECT

public:
    explicit ExplorerWidget(QWidget* parent = nullptr);
    ~ExplorerWidget() override;

    [[nodiscard]] ExplorerFolderEntry* AddFolder(QString name);

private slots:
    void HandleItemDoubleClicked();
    void HandleNavigateUpRequested();
    void HandleContextMenuRequested(const QPoint& position);
    void HandlePendingFolderSubmitted();
    void HandlePendingFolderCancelled();
    void HandlePendingFolderNameChanged(const QString& text);
    void RefreshSelectionState();

private:
    bool eventFilter(QObject* watched, QEvent* event) override;
    bool FinalizePendingFolderCreation();
    void BeginFolderCreation();
    void ClearPendingFolderCreation();
    void ConnectCurrentFolder();
    void RefreshCurrentFolderView();
    void UpdateNavigationUi();
    [[nodiscard]] ExplorerEntry* GetEntryForCurrentItem() const;
    [[nodiscard]] ExplorerEntry* GetEntryForItem(const QListWidgetItem* item) const;

    ExplorerFolderEntry* rootFolder_ {nullptr};
    ExplorerFolderEntry* currentFolder_ {nullptr};
    QPushButton* navigateUpButton_ {nullptr};
    QLabel* currentPathLabel_ {nullptr};
    QListWidget* contentList_ {nullptr};
    QPointer<ExplorerItemWidget> pendingFolderWidget_ {nullptr};
};

} // namespace Mirage::EditorQt
