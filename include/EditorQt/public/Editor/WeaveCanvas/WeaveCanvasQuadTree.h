#pragma once

#include "WaveGraph/WeaveGraphTypes.hpp"

#include <QPointF>
#include <QRectF>
#include <QVector>

#include <array>
#include <memory>

struct WeaveCanvasNodeViewData;
struct WeaveCanvasRenderNode;

class WeaveCanvasQuadTree final {
public:
    explicit WeaveCanvasQuadTree(
        const QRectF& worldBounds = DefaultWorldBounds(),
        int maxEntriesPerNode = 4,
        int maxDepth = 8
    );

    void Clear();
    void SetWorldBounds(const QRectF& worldBounds);
    void Rebuild(const QVector<WeaveCanvasRenderNode>& renderNodes);
    [[nodiscard]] bool Insert(const WeaveCanvasRenderNode& renderNode);
    [[nodiscard]] bool Reindex(const WeaveCanvasRenderNode& renderNode);
    [[nodiscard]] const WeaveCanvasRenderNode* HitScan(const QPointF& worldPosition) const;
    [[nodiscard]] QVector<const WeaveCanvasRenderNode*> Query(const QRectF& worldRect) const;

    [[nodiscard]] static QRectF BuildNodeWorldRect(const WeaveCanvasNodeViewData& node);
    [[nodiscard]] static QRectF DefaultWorldBounds();

private:
    struct Entry {
        const WeaveCanvasRenderNode* renderNode {nullptr};
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
        const WeaveCanvasRenderNode* renderNode {nullptr};
        quint64 insertionIndex {0};
    };

    [[nodiscard]] bool Insert(Node& node, Entry entry);
    [[nodiscard]] bool ExtractEntry(Node& node, Mirage::EditorCore::WaveGraph::WeaveNodeId nodeId, Entry& entry);
    void Subdivide(Node& node);
    void RedistributeEntries(Node& node);
    [[nodiscard]] int FindContainingChildIndex(const Node& node, const QRectF& rect) const;
    [[nodiscard]] HitResult HitScan(const Node& node, const QPointF& worldPosition) const;
    void Query(const Node& node, const QRectF& worldRect, QVector<const WeaveCanvasRenderNode*>& results) const;

    QRectF worldBounds_ {};
    int maxEntriesPerNode_ {4};
    int maxDepth_ {8};
    quint64 nextInsertionIndex_ {0};
    std::unique_ptr<Node> root_ {};
};
