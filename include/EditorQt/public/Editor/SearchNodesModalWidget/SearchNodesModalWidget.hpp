#pragma once

#include <QHash>
#include "WaveGraph/WeaveNodeRegistryTypes.hpp"

#include <QWidget>

class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;
class QString;

namespace Mirage::EditorQt {

class SearchNodesModalWidget final : public QWidget {
    Q_OBJECT
public:
    explicit SearchNodesModalWidget(
        const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& rootGroup,
        QWidget* parent = nullptr
    );

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void OnShouldClose();
    void OnNodeSelected(Mirage::EditorCore::WaveGraph::WeaveNodeInfo nodeInfo);

private:
    void ApplyStyles();
    void BuildUi();
    void PopulateTree();
    [[nodiscard]] QWidget* CreateGroupItemWidget(
        QTreeWidgetItem* item,
        const QString& labelText,
        int depth
    );
    [[nodiscard]] QWidget* CreateNodeItemWidget(const QString& labelText, int depth);
    [[nodiscard]] int ComputeTreeDepth(const QTreeWidgetItem* item) const;
    void AddGroupToTree(
        const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& group,
        QTreeWidgetItem* parentItem
    );
    void AddNodeToTree(
        const Mirage::EditorCore::WaveGraph::WeaveNodeInfo& node,
        QTreeWidgetItem* parentItem
    );
    [[nodiscard]] QString BuildGroupLabel(
        const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& group
    ) const;

    const Mirage::EditorCore::WaveGraph::WeaveNodeGroupInfo& rootGroup_;
    QLabel* titleLabel_ {nullptr};
    QLabel* subtitleLabel_ {nullptr};
    QLineEdit* searchLineEdit_ {nullptr};
    QTreeWidget* treeWidget_ {nullptr};
    QLabel* keyboardHintLabel_ {nullptr};
    QLabel* escapeHintLabel_ {nullptr};
    QHash<QTreeWidgetItem*, QLabel*> groupArrowLabels_ {};
};

} // namespace Mirage::EditorQt
