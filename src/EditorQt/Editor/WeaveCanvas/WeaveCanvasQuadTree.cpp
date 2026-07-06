// #include "Editor/WeaveCanvas/WeaveCanvasQuadTree.h"
//
// #include "Editor/WeaveCanvas/WeaveCanvas.h"
// #include "Editor/WeaveCanvas/WeaveNodePainter.h"
//
// #include <algorithm>
// #include <utility>
//
// namespace {
//
// constexpr qreal kDefaultWorldMin = -100000.0;
// constexpr qreal kDefaultWorldMax = 100000.0;
//
// QRectF BuildChildBounds(const QRectF& parentBounds, const int childIndex) {
//     const qreal halfWidth = parentBounds.width() * 0.5;
//     const qreal halfHeight = parentBounds.height() * 0.5;
//
//     switch (childIndex) {
//         case 0:
//             return {parentBounds.left(), parentBounds.top(), halfWidth, halfHeight};
//         case 1:
//             return {parentBounds.left() + halfWidth, parentBounds.top(), halfWidth, halfHeight};
//         case 2:
//             return {parentBounds.left(), parentBounds.top() + halfHeight, halfWidth, halfHeight};
//         case 3:
//         default:
//             return {
//                 parentBounds.left() + halfWidth,
//                 parentBounds.top() + halfHeight,
//                 halfWidth,
//                 halfHeight
//             };
//     }
// }
//
// } // namespace
//
// WeaveCanvasQuadTree::WeaveCanvasQuadTree(
//     const QRectF& worldBounds,
//     const int maxEntriesPerNode,
//     const int maxDepth
// )
//     : worldBounds_(worldBounds)
//     , maxEntriesPerNode_(std::max(1, maxEntriesPerNode))
//     , maxDepth_(std::max(1, maxDepth))
//     , root_(std::make_unique<Node>()) {
//     root_->bounds = worldBounds_;
//     root_->depth = 0;
// }
//
// void WeaveCanvasQuadTree::Clear() {
//     nextInsertionIndex_ = 0;
//     root_ = std::make_unique<Node>();
//     root_->bounds = worldBounds_;
//     root_->depth = 0;
// }
//
// void WeaveCanvasQuadTree::SetWorldBounds(const QRectF& worldBounds) {
//     worldBounds_ = worldBounds;
//     Clear();
// }
//
// void WeaveCanvasQuadTree::Rebuild(const QVector<WeaveCanvasRenderNode>& renderNodes) {
//     Clear();
//
//     for (const WeaveCanvasRenderNode& renderNode : renderNodes) {
//         Insert(renderNode);
//     }
// }
//
// bool WeaveCanvasQuadTree::Insert(const WeaveCanvasRenderNode& renderNode) {
//     if (root_ == nullptr) {
//         return false;
//     }
//
//     const QRectF worldRect = BuildNodeWorldRect(renderNode.viewData);
//     if (!worldBounds_.contains(worldRect)) {
//         return false;
//     }
//
//     return Insert(
//         *root_,
//         Entry{
//             .renderNode = &renderNode,
//             .worldRect = worldRect,
//             .insertionIndex = nextInsertionIndex_++,
//         }
//     );
// }
//
// bool WeaveCanvasQuadTree::Reindex(const WeaveCanvasRenderNode& renderNode) {
//     if (root_ == nullptr) {
//         return false;
//     }
//
//     const QRectF worldRect = BuildNodeWorldRect(renderNode.viewData);
//     if (!worldBounds_.contains(worldRect)) {
//         return false;
//     }
//
//     Entry entry {
//         .renderNode = &renderNode,
//         .worldRect = worldRect,
//         .insertionIndex = nextInsertionIndex_++,
//     };
//
//     if (ExtractEntry(*root_, renderNode.nodeId, entry)) {
//         entry.renderNode = &renderNode;
//         entry.worldRect = worldRect;
//     }
//
//     return Insert(*root_, std::move(entry));
// }
//
// const WeaveCanvasRenderNode* WeaveCanvasQuadTree::HitScan(const QPointF& worldPosition) const {
//     if (root_ == nullptr || !worldBounds_.contains(worldPosition)) {
//         return nullptr;
//     }
//
//     return HitScan(*root_, worldPosition).renderNode;
// }
//
// QVector<const WeaveCanvasRenderNode*> WeaveCanvasQuadTree::Query(const QRectF& worldRect) const {
//     QVector<const WeaveCanvasRenderNode*> results;
//
//     if (root_ == nullptr || !worldBounds_.intersects(worldRect)) {
//         return results;
//     }
//
//     Query(*root_, worldRect, results);
//     return results;
// }
//
// QRectF WeaveCanvasQuadTree::BuildNodeWorldRect(const WeaveCanvasNodeViewData& node) {
//     const float nodeHeight = std::max(node.size.y, WeaveNodePainter::CalculateNodeBodyHeight(node));
//     return {
//         static_cast<qreal>(node.position.x - node.size.x * 0.5F),
//         static_cast<qreal>(node.position.y - nodeHeight * 0.5F),
//         static_cast<qreal>(node.size.x),
//         static_cast<qreal>(nodeHeight)
//     };
// }
//
// QRectF WeaveCanvasQuadTree::DefaultWorldBounds() {
//     return {kDefaultWorldMin, kDefaultWorldMin, kDefaultWorldMax - kDefaultWorldMin, kDefaultWorldMax - kDefaultWorldMin};
// }
//
// bool WeaveCanvasQuadTree::Insert(Node& node, Entry entry) {
//     if (!node.bounds.contains(entry.worldRect)) {
//         return false;
//     }
//
//     if (!node.IsLeaf()) {
//         const int childIndex = FindContainingChildIndex(node, entry.worldRect);
//         if (childIndex >= 0) {
//             return Insert(*node.children[static_cast<std::size_t>(childIndex)], std::move(entry));
//         }
//     }
//
//     node.entries.push_back(std::move(entry));
//
//     if (node.entries.size() > maxEntriesPerNode_ && node.depth < maxDepth_) {
//         if (node.IsLeaf()) {
//             Subdivide(node);
//         }
//         RedistributeEntries(node);
//     }
//
//     return true;
// }
//
// bool WeaveCanvasQuadTree::ExtractEntry(
//     Node& node,
//     const Mirage::EditorCore::WaveGraph::WeaveNodeId nodeId,
//     Entry& entry
// ) {
//     for (auto it = node.entries.begin(); it != node.entries.end(); ++it) {
//         if (it->renderNode != nullptr && it->renderNode->nodeId == nodeId) {
//             entry = std::move(*it);
//             node.entries.erase(it);
//             return true;
//         }
//     }
//
//     if (node.IsLeaf()) {
//         return false;
//     }
//
//     for (auto& child : node.children) {
//         if (child != nullptr && ExtractEntry(*child, nodeId, entry)) {
//             return true;
//         }
//     }
//
//     return false;
// }
//
// void WeaveCanvasQuadTree::Subdivide(Node& node) {
//     if (!node.IsLeaf()) {
//         return;
//     }
//
//     for (int childIndex = 0; childIndex < 4; ++childIndex) {
//         auto child = std::make_unique<Node>();
//         child->bounds = BuildChildBounds(node.bounds, childIndex);
//         child->depth = node.depth + 1;
//         node.children[static_cast<std::size_t>(childIndex)] = std::move(child);
//     }
// }
//
// void WeaveCanvasQuadTree::RedistributeEntries(Node& node) {
//     if (node.IsLeaf()) {
//         return;
//     }
//
//     QVector<Entry> remainingEntries;
//     remainingEntries.reserve(node.entries.size());
//
//     for (Entry& entry : node.entries) {
//         const int childIndex = FindContainingChildIndex(node, entry.worldRect);
//         if (childIndex >= 0) {
//             Insert(*node.children[static_cast<std::size_t>(childIndex)], std::move(entry));
//         } else {
//             remainingEntries.push_back(std::move(entry));
//         }
//     }
//
//     node.entries = std::move(remainingEntries);
// }
//
// int WeaveCanvasQuadTree::FindContainingChildIndex(const Node& node, const QRectF& rect) const {
//     if (node.IsLeaf()) {
//         return -1;
//     }
//
//     for (int childIndex = 0; childIndex < 4; ++childIndex) {
//         const auto& child = node.children[static_cast<std::size_t>(childIndex)];
//         if (child != nullptr && child->bounds.contains(rect)) {
//             return childIndex;
//         }
//     }
//
//     return -1;
// }
//
// WeaveCanvasQuadTree::HitResult WeaveCanvasQuadTree::HitScan(const Node& node, const QPointF& worldPosition) const {
//     HitResult bestResult {};
//
//     for (auto it = node.entries.crbegin(); it != node.entries.crend(); ++it) {
//         if (it->worldRect.contains(worldPosition) && it->insertionIndex >= bestResult.insertionIndex) {
//             bestResult = {
//                 .renderNode = it->renderNode,
//                 .insertionIndex = it->insertionIndex,
//             };
//         }
//     }
//
//     if (node.IsLeaf()) {
//         return bestResult;
//     }
//
//     for (const auto& child : node.children) {
//         if (child == nullptr || !child->bounds.contains(worldPosition)) {
//             continue;
//         }
//
//         const HitResult childResult = HitScan(*child, worldPosition);
//         if (childResult.renderNode != nullptr && childResult.insertionIndex >= bestResult.insertionIndex) {
//             bestResult = childResult;
//         }
//         break;
//     }
//
//     return bestResult;
// }
//
// void WeaveCanvasQuadTree::Query(
//     const Node& node,
//     const QRectF& worldRect,
//     QVector<const WeaveCanvasRenderNode*>& results
// ) const {
//     if (!node.bounds.intersects(worldRect)) {
//         return;
//     }
//
//     for (const Entry& entry : node.entries) {
//         if (entry.worldRect.intersects(worldRect)) {
//             results.push_back(entry.renderNode);
//         }
//     }
//
//     if (node.IsLeaf()) {
//         return;
//     }
//
//     for (const auto& child : node.children) {
//         if (child != nullptr) {
//             Query(*child, worldRect, results);
//         }
//     }
// }
