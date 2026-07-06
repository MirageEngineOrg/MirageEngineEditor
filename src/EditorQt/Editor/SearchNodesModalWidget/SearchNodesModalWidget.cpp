#include "Editor/SearchNodesModalWidget/SearchNodesModalWidget.hpp"

#include "Editor/SearchNodesModalWidget/SearchNodesModalWidgetInternal.hpp"

#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QShortcut>
#include <QSizePolicy>
#include <QStyle>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace Mirage::EditorQt {

namespace {

constexpr auto kGroupItemType = QTreeWidgetItem::UserType + 1;
constexpr auto kNodeItemType = QTreeWidgetItem::UserType + 2;
constexpr int kTreeDepthIndent = 24;

} // namespace

SearchNodesModalWidget::SearchNodesModalWidget(
    const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& rootGroup,
    QWidget* parent
)
    : QWidget(parent)
    , rootGroup_(rootGroup) {
    BuildUi();
    ApplyStyles();
    PopulateTree();
}
void SearchNodesModalWidget::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
}

void SearchNodesModalWidget::ApplyStyles() {
    QFile styleSheetFile(QString::fromUtf8(kSearchNodesModalStyleSheetPath));
    if (!styleSheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    setStyleSheet(QString::fromUtf8(styleSheetFile.readAll()));
}

void SearchNodesModalWidget::BuildUi() {
    setObjectName("SearchNodesModalRoot");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumSize(kSearchNodesModalMinimumWidth, kSearchNodesModalMinimumHeight);
    setMaximumSize(kSearchNodesModalWidth, kSearchNodesModalHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFocusPolicy(Qt::StrongFocus);

    auto* closeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    closeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(closeShortcut, &QShortcut::activated, this, &SearchNodesModalWidget::OnShouldClose);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(28, 24, 28, 24);
    rootLayout->setSpacing(0);

    titleLabel_ = new QLabel("Add Node", this);
    titleLabel_->setObjectName("SearchNodesModalTitle");
    rootLayout->addWidget(titleLabel_);

    subtitleLabel_ = new QLabel(
        "Browse groups, drill into subgroups, or search by node name.",
        this
    );
    subtitleLabel_->setObjectName("SearchNodesModalSubtitle");
    rootLayout->addWidget(subtitleLabel_);

    rootLayout->addSpacing(19);

    searchLineEdit_ = new QLineEdit(this);
    searchLineEdit_->setObjectName("SearchNodesModalSearchField");
    searchLineEdit_->setPlaceholderText("Search nodes, functions, events, variables...");
    searchLineEdit_->setClearButtonEnabled(false);
    searchLineEdit_->setEnabled(false);
    rootLayout->addWidget(searchLineEdit_);

    rootLayout->addSpacing(20);

    auto* treePanel = new QFrame(this);
    treePanel->setObjectName("SearchNodesModalTreePanel");
    treePanel->setMaximumHeight(kTreePanelPreferredHeight);
    treePanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* treePanelLayout = new QVBoxLayout(treePanel);
    treePanelLayout->setContentsMargins(16, 14, 10, 12);
    treePanelLayout->setSpacing(10);

    auto* treeHintLabel = new QLabel("Node Tree", treePanel);
    treeHintLabel->setObjectName("SearchNodesModalTreeHint");
    treePanelLayout->addWidget(treeHintLabel);

    treeWidget_ = new QTreeWidget(treePanel);
    treeWidget_->setObjectName("SearchNodesModalTree");
    treeWidget_->setHeaderHidden(true);
    treeWidget_->setRootIsDecorated(false);
    treeWidget_->setIndentation(0);
    treeWidget_->setUniformRowHeights(false);
    treeWidget_->setAnimated(false);
    treeWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    treeWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    treeWidget_->header()->setStretchLastSection(true);
    connect(treeWidget_, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item) {
        if (QLabel* arrowLabel = groupArrowLabels_.value(item, nullptr); arrowLabel != nullptr) {
            arrowLabel->setText(QString::fromUtf8("\u25BE"));
        }
    });
    connect(treeWidget_, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem* item) {
        if (QLabel* arrowLabel = groupArrowLabels_.value(item, nullptr); arrowLabel != nullptr) {
            arrowLabel->setText(QString::fromUtf8("\u25B8"));
        }
    });
    connect(treeWidget_, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int) {
        if (item != nullptr && item->type() == kGroupItemType) {
            item->setExpanded(!item->isExpanded());
        }
    });
    treePanelLayout->addWidget(treeWidget_, 1);

    rootLayout->addWidget(treePanel, 1);

    rootLayout->addSpacing(18);

    auto* footerContainer = new QWidget(this);
    footerContainer->setObjectName("SearchNodesModalFooter");
    footerContainer->setFixedHeight(kSearchNodesModalFooterHeight);
    footerContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto* footerLayout = new QHBoxLayout(footerContainer);
    footerLayout->setContentsMargins(4, 0, 4, 0);
    footerLayout->setSpacing(8);

    keyboardHintLabel_ = new QLabel(
        "Type to filter. Enter to place the selected node.",
        this
    );
    keyboardHintLabel_->setObjectName("SearchNodesModalKeyboardHint");
    footerLayout->addWidget(keyboardHintLabel_);
    footerLayout->addStretch(1);

    escapeHintLabel_ = new QLabel("Esc closes", this);
    escapeHintLabel_->setObjectName("SearchNodesModalEscapeHint");
    footerLayout->addWidget(escapeHintLabel_);

    rootLayout->addSpacing(18);
    rootLayout->addWidget(footerContainer);
    rootLayout->addSpacing(kSearchNodesModalFooterBottomSpacing);
}

void SearchNodesModalWidget::PopulateTree() {
    treeWidget_->clear();

    if (!rootGroup_.name.empty()) {
        AddGroupToTree(rootGroup_, nullptr);
    } else {
        for (const auto& subGroup : rootGroup_.subGroups) {
            AddGroupToTree(subGroup, nullptr);
        }

        for (const auto& node : rootGroup_.nodes) {
            AddNodeToTree(node, nullptr);
        }
    }

    treeWidget_->expandToDepth(1);
}

QWidget* SearchNodesModalWidget::CreateGroupItemWidget(
    QTreeWidgetItem* item,
    const QString& labelText,
    const int depth
) {
    auto* container = new QWidget(treeWidget_);
    auto* layout = new QHBoxLayout(container);
    layout->setContentsMargins(depth * kTreeDepthIndent, 0, 0, 0);
    layout->setSpacing(6);

    auto* arrowLabel = new QLabel(QString::fromUtf8("\u25BE"), container);
    arrowLabel->setObjectName("SearchNodesModalTreeArrow");
    arrowLabel->setFixedWidth(10);
    layout->addWidget(arrowLabel);

    auto* textLabel = new QLabel(labelText, container);
    textLabel->setObjectName("SearchNodesModalTreeGroupLabel");
    layout->addWidget(textLabel);
    layout->addStretch(1);

    groupArrowLabels_.insert(item, arrowLabel);
    return container;
}

QWidget* SearchNodesModalWidget::CreateNodeItemWidget(const QString& labelText, const int depth) {
    auto* container = new QWidget(treeWidget_);
    auto* layout = new QHBoxLayout(container);
    layout->setContentsMargins(depth * kTreeDepthIndent, 0, 0, 0);
    layout->setSpacing(0);

    auto* textLabel = new QLabel(labelText, container);
    textLabel->setObjectName("SearchNodesModalTreeNodeLabel");
    layout->addWidget(textLabel);
    layout->addStretch(1);

    return container;
}

int SearchNodesModalWidget::ComputeTreeDepth(const QTreeWidgetItem* item) const {
    int depth = 0;
    const QTreeWidgetItem* currentItem = item != nullptr ? item->parent() : nullptr;
    while (currentItem != nullptr) {
        ++depth;
        currentItem = currentItem->parent();
    }

    return depth;
}

void SearchNodesModalWidget::AddGroupToTree(
    const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& group,
    QTreeWidgetItem* parentItem
) {
    auto* groupItem = new QTreeWidgetItem(kGroupItemType);
    groupItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    groupItem->setExpanded(true);
    groupItem->setSizeHint(0, QSize(0, 34));

    if (parentItem == nullptr) {
        treeWidget_->addTopLevelItem(groupItem);
    } else {
        parentItem->addChild(groupItem);
    }

    treeWidget_->setItemWidget(
        groupItem,
        0,
        CreateGroupItemWidget(groupItem, BuildGroupLabel(group), ComputeTreeDepth(groupItem))
    );

    for (const auto& subGroup : group.subGroups) {
        AddGroupToTree(subGroup, groupItem);
    }

    for (const auto& node : group.nodes) {
        AddNodeToTree(node, groupItem);
    }
}

void SearchNodesModalWidget::AddNodeToTree(
    const Mirage::EditorCore::WaveGraph::WeaveNodeInfo& node,
    QTreeWidgetItem* parentItem
) {
    auto* nodeItem = new QTreeWidgetItem(kNodeItemType);
    nodeItem->setToolTip(0, QString::fromStdString(node.description));
    nodeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    nodeItem->setSizeHint(0, QSize(0, 34));

    if (parentItem == nullptr) {
        treeWidget_->addTopLevelItem(nodeItem);
    } else {
        parentItem->addChild(nodeItem);
    }

    treeWidget_->setItemWidget(
        nodeItem,
        0,
        CreateNodeItemWidget(QString::fromStdString(node.name), ComputeTreeDepth(nodeItem))
    );

    connect(treeWidget_, &QTreeWidget::itemClicked, this, [this, nodeItem, node](QTreeWidgetItem* clickedItem, int) {
        if (clickedItem == nodeItem) {
            emit OnNodeSelected(node);
        }
    });
}

QString SearchNodesModalWidget::BuildGroupLabel(
    const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& group
) const {
    return QString::fromStdString(group.name);
}

} // namespace Mirage::EditorQt
