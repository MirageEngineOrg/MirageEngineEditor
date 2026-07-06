#include "Explorer/ExplorerWidget.hpp"

#include "Explorer/ExplorerEntry.hpp"
#include "Explorer/ExplorerFileEntry.hpp"
#include "Explorer/ExplorerFileWidget.hpp"
#include "Explorer/ExplorerFolderEntry.hpp"
#include "Explorer/ExplorerFolderWidget.hpp"

#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QMenu>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTimer>
#include <QVariant>
#include <QVBoxLayout>

namespace Mirage::EditorQt {

namespace {

constexpr int kEntryPointerRole = Qt::UserRole;

QString BuildFolderPath(const ExplorerFolderEntry* folder) {
    QStringList segments;
    for (auto* current = folder; current != nullptr; current = current->GetParentFolder()) {
        segments.prepend(current->GetName());
    }

    return segments.join('/');
}

} // namespace

ExplorerWidget::ExplorerWidget(QWidget* parent)
    : QWidget(parent) {
    rootFolder_ = new ExplorerFolderEntry("Root", nullptr);
    currentFolder_ = rootFolder_;

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(8);

    auto* toolbarWidget = new QWidget(this);
    auto* toolbarLayout = new QHBoxLayout(toolbarWidget);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarLayout->setSpacing(8);

    navigateUpButton_ = new QPushButton("Up", toolbarWidget);
    currentPathLabel_ = new QLabel(toolbarWidget);

    toolbarLayout->addWidget(navigateUpButton_);
    toolbarLayout->addWidget(currentPathLabel_, 1);

    contentList_ = new QListWidget(this);
    contentList_->setViewMode(QListView::IconMode);
    contentList_->setResizeMode(QListView::Adjust);
    contentList_->setMovement(QListView::Static);
    contentList_->setSpacing(10);
    contentList_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    contentList_->setUniformItemSizes(false);
    contentList_->setWrapping(true);
    contentList_->setContextMenuPolicy(Qt::CustomContextMenu);

    rootLayout->addWidget(toolbarWidget);
    rootLayout->addWidget(contentList_, 1);

    connect(navigateUpButton_, &QPushButton::clicked,
            this, &ExplorerWidget::HandleNavigateUpRequested);
    connect(contentList_, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem*) { HandleItemDoubleClicked(); });
    connect(contentList_, &QListWidget::customContextMenuRequested,
            this, &ExplorerWidget::HandleContextMenuRequested);
    connect(contentList_, &QListWidget::itemSelectionChanged,
            this, &ExplorerWidget::RefreshSelectionState);

    qApp->installEventFilter(this);

    ConnectCurrentFolder();
    RefreshCurrentFolderView();
}

ExplorerWidget::~ExplorerWidget() {
    qApp->removeEventFilter(this);
}

ExplorerFolderEntry* ExplorerWidget::AddFolder(QString name) {
    return rootFolder_->AddFolder(std::move(name));
}

void ExplorerWidget::HandleItemDoubleClicked() {
    ExplorerEntry* entry = GetEntryForCurrentItem();
    if (entry == nullptr || entry->GetKind() != ExplorerEntry::Kind::Folder) {
        return;
    }

    currentFolder_ = static_cast<ExplorerFolderEntry*>(entry);
    ConnectCurrentFolder();
    RefreshCurrentFolderView();
}

void ExplorerWidget::HandleNavigateUpRequested() {
    if (currentFolder_ == nullptr) {
        return;
    }

    ExplorerFolderEntry* parentFolder = currentFolder_->GetParentFolder();
    if (parentFolder == nullptr) {
        return;
    }

    currentFolder_ = parentFolder;
    ConnectCurrentFolder();
    RefreshCurrentFolderView();
}

void ExplorerWidget::HandleContextMenuRequested(const QPoint& position) {
    if (contentList_ == nullptr) {
        return;
    }

    QListWidgetItem* clickedItem = contentList_->itemAt(position);
    if (clickedItem != nullptr && !clickedItem->isSelected()) {
        contentList_->clearSelection();
        clickedItem->setSelected(true);
        contentList_->setCurrentItem(clickedItem);
    }

    QMenu contextMenu(this);

    if (clickedItem == nullptr) {
        QAction* addFolderAction = contextMenu.addAction("Add Folder");
        QAction* selectedAction = contextMenu.exec(contentList_->viewport()->mapToGlobal(position));
        if (selectedAction == addFolderAction) {
            QTimer::singleShot(0, this, &ExplorerWidget::BeginFolderCreation);
        }
        return;
    }

    contextMenu.addAction("Open");
    contextMenu.addAction("Rename");
    contextMenu.addAction("Delete");
    QAction* selectedAction = contextMenu.exec(contentList_->viewport()->mapToGlobal(position));
    Q_UNUSED(selectedAction);
}

void ExplorerWidget::HandlePendingFolderSubmitted() {
    FinalizePendingFolderCreation();
}

void ExplorerWidget::HandlePendingFolderCancelled() {
    ClearPendingFolderCreation();
}

void ExplorerWidget::HandlePendingFolderNameChanged(const QString& text) {
    if (pendingFolderWidget_ == nullptr || currentFolder_ == nullptr) {
        return;
    }

    const QString trimmedName = text.trimmed();
    const bool hasConflict = trimmedName.isEmpty() || currentFolder_->HasChildNamed(trimmedName);
    pendingFolderWidget_->SetInvalid(hasConflict);
}

void ExplorerWidget::RefreshSelectionState() {
    if (contentList_ == nullptr) {
        return;
    }

    for (int index = 0; index < contentList_->count(); ++index) {
        QListWidgetItem* item = contentList_->item(index);
        QWidget* itemWidget = contentList_->itemWidget(item);
        auto* explorerItemWidget = dynamic_cast<ExplorerItemWidget*>(itemWidget);
        if (explorerItemWidget == nullptr) {
            continue;
        }

        explorerItemWidget->SetSelected(item->isSelected());
    }
}

bool ExplorerWidget::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);

    if (pendingFolderWidget_ == nullptr) {
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (!rect().contains(mapFromGlobal(mouseEvent->globalPosition().toPoint()))) {
            FinalizePendingFolderCreation();
        }
    } else if (event->type() == QEvent::WindowDeactivate) {
        FinalizePendingFolderCreation();
    }

    return QWidget::eventFilter(watched, event);
}

bool ExplorerWidget::FinalizePendingFolderCreation() {
    if (currentFolder_ == nullptr || pendingFolderWidget_ == nullptr) {
        return false;
    }

    const QString trimmedName = pendingFolderWidget_->GetEditedName().trimmed();
    const bool isValid = !trimmedName.isEmpty() && !currentFolder_->HasChildNamed(trimmedName);
    pendingFolderWidget_->SetInvalid(!isValid);
    if (!isValid) {
        ClearPendingFolderCreation();
        return false;
    }

    const auto* createdFolder = currentFolder_->AddFolder(trimmedName);
    Q_UNUSED(createdFolder);
    ClearPendingFolderCreation();
    RefreshCurrentFolderView();
    return true;
}

void ExplorerWidget::ConnectCurrentFolder() {
    if (currentFolder_ == nullptr) {
        return;
    }

    connect(
        currentFolder_,
        &ExplorerFolderEntry::ContentChanged,
        this,
        &ExplorerWidget::RefreshCurrentFolderView,
        Qt::UniqueConnection
    );
}

void ExplorerWidget::BeginFolderCreation() {
    if (contentList_ == nullptr || currentFolder_ == nullptr) {
        return;
    }

    ClearPendingFolderCreation();

    auto* pendingFolderItem = new QListWidgetItem(contentList_);
    pendingFolderItem->setSizeHint(QSize(110, 84));
    pendingFolderWidget_ = new ExplorerFolderWidget("New Folder", contentList_);
    pendingFolderWidget_->SetSelected(false);
    pendingFolderWidget_->SetInvalid(false);
    contentList_->setItemWidget(pendingFolderItem, pendingFolderWidget_);

    connect(pendingFolderWidget_, &ExplorerItemWidget::EditingSubmitted,
            this, &ExplorerWidget::HandlePendingFolderSubmitted, Qt::UniqueConnection);
    connect(pendingFolderWidget_, &ExplorerItemWidget::EditingCancelled,
            this, &ExplorerWidget::HandlePendingFolderCancelled, Qt::UniqueConnection);
    connect(pendingFolderWidget_, &ExplorerItemWidget::EditingTextChanged,
            this, &ExplorerWidget::HandlePendingFolderNameChanged, Qt::UniqueConnection);

    pendingFolderWidget_->StartEditing();
}

void ExplorerWidget::ClearPendingFolderCreation() {
    if (contentList_ != nullptr && pendingFolderWidget_ != nullptr) {
        for (int row = 0; row < contentList_->count(); ++row) {
            QListWidgetItem* item = contentList_->item(row);
            if (contentList_->itemWidget(item) != pendingFolderWidget_) {
                continue;
            }

            contentList_->removeItemWidget(item);
            delete contentList_->takeItem(row);
            break;
        }
    }

    pendingFolderWidget_ = nullptr;
}

void ExplorerWidget::RefreshCurrentFolderView() {
    if (contentList_ == nullptr || currentFolder_ == nullptr) {
        return;
    }

    if (pendingFolderWidget_ != nullptr) {
        return;
    }

    const QSignalBlocker blocker(contentList_);
    contentList_->clear();

    for (ExplorerEntry* entry : currentFolder_->GetChildren()) {
        auto* listItem = new QListWidgetItem(contentList_);
        listItem->setData(kEntryPointerRole, QVariant::fromValue(reinterpret_cast<quintptr>(entry)));
        listItem->setSizeHint(QSize(110, 84));

        QWidget* tileWidget = nullptr;
        if (entry->GetKind() == ExplorerEntry::Kind::Folder) {
            tileWidget = new ExplorerFolderWidget(entry->GetName(), contentList_);
        } else {
            tileWidget = new ExplorerFileWidget(entry->GetName(), contentList_);
        }

        contentList_->setItemWidget(listItem, tileWidget);
    }

    UpdateNavigationUi();
    RefreshSelectionState();
}

void ExplorerWidget::UpdateNavigationUi() {
    if (navigateUpButton_ != nullptr) {
        navigateUpButton_->setEnabled(
            currentFolder_ != nullptr && currentFolder_->GetParentFolder() != nullptr
        );
    }

    if (currentPathLabel_ != nullptr && currentFolder_ != nullptr) {
        currentPathLabel_->setText(BuildFolderPath(currentFolder_));
    }
}

ExplorerEntry* ExplorerWidget::GetEntryForCurrentItem() const {
    if (contentList_ == nullptr) {
        return nullptr;
    }

    QListWidgetItem* currentItem = contentList_->currentItem();
    if (currentItem == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<ExplorerEntry*>(
        currentItem->data(kEntryPointerRole).value<quintptr>()
    );
}

ExplorerEntry* ExplorerWidget::GetEntryForItem(const QListWidgetItem* item) const {
    if (item == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<ExplorerEntry*>(
        item->data(kEntryPointerRole).value<quintptr>()
    );
}

} // namespace Mirage::EditorQt
