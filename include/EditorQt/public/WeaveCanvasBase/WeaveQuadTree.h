#pragma once

#include "WeaveCanvasBase/RenderedItem/RenderedItem.h"

#include <QRectF>
#include <QVector>

#include <array>
#include <memory>

class WeaveQuadTree final {
public:
    explicit WeaveQuadTree(
        const QRectF& worldBounds = DefaultWorldBounds(),
        int maxEntriesPerNode = 4,
        int maxDepth = 8
    );

    void Clear();
    void SetWorldBounds(const QRectF& worldBounds);
    void Rebuild(const QVector<IRenderedItem*>& renderNodes);
    [[nodiscard]] bool Insert(IRenderedItem *renderNode);
    [[nodiscard]] bool Reindex(IRenderedItem *renderNode);
    [[nodiscard]] IRenderedItem *HitScan(const QPointF &worldPosition) const;
    [[nodiscard]] QVector<const IRenderedItem*> Query(const QRectF& worldRect) const;

    [[nodiscard]] static QRectF BuildNodeWorldRect(const IRenderedItem* node);
    [[nodiscard]] static QRectF DefaultWorldBounds();

private:
    struct Entry {
        IRenderedItem* renderNode {nullptr};
        QRectF worldRect {};
        quint64 insertionIndex {0};
    };

    struct Node {
        QRectF bounds {};
        int depth {0};
        QVector<Entry> entries {};
        std::array<std::unique_ptr<Node>, 4> children {};

        [[nodiscard]] bool IsLeaf() const noexcept {
            return children[0] == nullptr;
        }
    };

    struct HitResult {
        IRenderedItem* renderNode {nullptr};
        quint64 insertionIndex {0};
    };

    [[nodiscard]] bool Insert(Node& node, Entry entry);
    [[nodiscard]] static bool ExtractEntry(Node& node, RenderedItemID nodeId, Entry& entry);
    static void Subdivide(Node& node);
    void RedistributeEntries(Node& node);
    [[nodiscard]] static int FindContainingChildIndex(const Node& node, const QRectF& rect);
    [[nodiscard]] static HitResult HitScan(const Node& node, const QPointF& worldPosition);
    static void Query(const Node& node, const QRectF& worldRect, QVector<const IRenderedItem*>& results);

    QRectF worldBounds_ {};
    int maxEntriesPerNode_ {4};
    int maxDepth_ {8};
    quint64 nextInsertionIndex_ {0};
    std::unique_ptr<Node> root_ {};
};
