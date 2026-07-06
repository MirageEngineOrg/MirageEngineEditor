#include "Editor/WeaveCanvas/WeaveCanvas.h"
#include "Editor/Grid/WeaveEditorGrid.h"

#include <QDebug>
#include <QPen>
#include <QStringList>

WeaveCanvas::WeaveCanvas(QWidget* parent) : IBaseWeaveCanvas(parent, std::make_unique<WeaveEditorGrid>()){
}

void WeaveCanvas::AddNode(const WeaveNodeId nodeId, std::shared_ptr<RenderedWeaveNodeItem> node) {
    if (node == nullptr) {
        return;
    }

    renderedWaveNodes_.insert(node->GetID(), nodeId);
    renderedItemIdsByNodeId_.insert(nodeId, node->GetID());
    AddRenderedItem(std::move(node));
}

void WeaveCanvas::AddConnection(const WeaveCanvasConnection& connection) {
    if (!connection.IsValid()) {
        return;
    }

    connections_.insert(connection.connectionId, connection);
}

void WeaveCanvas::OnItemsDragStarted() {
    IBaseWeaveCanvas::OnItemsDragStarted();
}

void WeaveCanvas::HandleSelectionChanged() {
    QStringList selectedNodeIds;

    for (const RenderedItemID renderedItemId : GetSelectedRenderedItemIds()) {
        const auto it = renderedWaveNodes_.find(renderedItemId);
        if (it != renderedWaveNodes_.end()) {
            selectedNodeIds.push_back(QString::number(it.value()));
        }
    }

    qDebug() << "Selected weave nodes:" << selectedNodeIds.join(", ");
}

CanvasInteractionMode WeaveCanvas::onPressedItem(OnPressItemContext on_press_item_context,
                                                 IRenderedItem *item) {
    if (item != nullptr && this->renderedWaveNodes_.contains(item->GetID())) {
        auto* weaveNodeItem = static_cast<RenderedWeaveNodeItem*>(item);
        const Vector2D pressWorldPosition =
            global_from_screen(on_press_item_context.posX, on_press_item_context.posY);
        const QPointF pressContextWorldPoint(pressWorldPosition.x, pressWorldPosition.y);

        const int pin_id = weaveNodeItem->GetPinIdAtWorldPosition(pressContextWorldPoint, camera_->GetZoom());
        if (pin_id == -1) {
            return IBaseWeaveCanvas::onPressedItem(on_press_item_context, item);
        }

        draggingPinSourceNode = weaveNodeItem;
        draggingPinIndex_ = pin_id;
        static_cast<void>(draggingPinSourceNode->SetPinActive(pin_id, true));
        return CanvasInteractionMode::Idle;
    }
    return IBaseWeaveCanvas::onPressedItem(on_press_item_context, item);
}

void WeaveCanvas::PaintCanvas(QPainter &painter) {
    IBaseWeaveCanvas::PaintCanvas(painter);

    for (const WeaveCanvasConnection& connection : connections_) {
        const RenderedWeaveNodeItem* sourceNode = FindNodeById(connection.sourceNodeId);
        const RenderedWeaveNodeItem* targetNode = FindNodeById(connection.targetNodeId);
        if (sourceNode == nullptr || targetNode == nullptr) {
            continue;
        }

        DrawConnection(
            sourceNode->GetPinWorldPosition(connection.sourcePinId),
            targetNode->GetPinWorldPosition(connection.targetPinId),
            painter
        );
    }

    if (draggingPinSourceNode != nullptr && draggingPinIndex_ != -1) {
        DrawConnection(
            draggingPinSourceNode->GetPinWorldPosition(draggingPinIndex_),
            global_from_screen(draggingEnd.x(), draggingEnd.y()),
            painter
        );
    }
}

void WeaveCanvas::OnDrag(float mouseX, float mouseY) {
    if (draggingPinIndex_ == -1) {
        IBaseWeaveCanvas::OnDrag(mouseX, mouseY);
        return;
    }

    draggingEnd = QPointF(mouseX, mouseY);
    Vector2D global_end = global_from_screen(draggingEnd.x(), draggingEnd.y());
    auto* nodeUnderCursor = ResolvePressedRenderedItem(QPointF(global_end.x, global_end.y));

    if (nodeUnderCursor != nullptr && nodeUnderCursor != draggingPinSourceNode) {
        auto it = this->renderedWaveNodes_.find(nodeUnderCursor->GetID());
        if (it != this->renderedWaveNodes_.end()) {
            const auto* weaveNodeItem = static_cast<const RenderedWeaveNodeItem*>(this->renderedItems_[nodeUnderCursor->GetID()].get());
            if (weaveNodeItem != nullptr) {
                int targetPinIndex = weaveNodeItem->GetPinIdAtWorldPosition(QPointF(global_end.x, global_end.y), this->camera_->GetZoom());
                if (targetPinIndex != -1) {
                    bool canEstablish = false;
                    emit ValidateConnection({
                            .connectionId = 0,
                            .sourceNodeId = this->renderedWaveNodes_[draggingPinSourceNode->GetID()],
                            .sourcePinId = draggingPinIndex_,
                            .targetNodeId = this->renderedWaveNodes_[weaveNodeItem->GetID()],
                            .targetPinId = targetPinIndex
                        }, canEstablish);
                    if (canEstablish) {
                        qDebug() << "WANT TO CONNECT TO "  << targetPinIndex;
                    } else {
                        qDebug() << "Connection validation failed"  << targetPinIndex;
                    }
                }
            }
        }
    }


    Update();
}
void WeaveCanvas::OnLeftMouseReleased(float mouseX, float mouseY) {
    if (draggingPinIndex_ == -1) {
        IBaseWeaveCanvas::OnLeftMouseReleased(mouseX, mouseY);
        return;
    }
    Update();
    draggingPinIndex_ = -1;
}

void WeaveCanvas::DrawConnection(const Vector2D &from_global, const Vector2D &to_global, QPainter& painter) {
    const QPointF fromScreenPoint =
        ScreenFromGlobalRect(QRectF(from_global.x, from_global.y, 0.0, 0.0)).topLeft();
    const QPointF toScreenPoint =
        ScreenFromGlobalRect(QRectF(to_global.x, to_global.y, 0.0, 0.0)).topLeft();

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(212, 175, 55), 2.0));
    painter.drawLine(fromScreenPoint, toScreenPoint);
    painter.restore();
}

RenderedWeaveNodeItem* WeaveCanvas::FindNodeById(const WeaveNodeId nodeId) const {
    const auto renderedItemIdIt = renderedItemIdsByNodeId_.find(nodeId);
    if (renderedItemIdIt == renderedItemIdsByNodeId_.end()) {
        return nullptr;
    }

    const auto renderedItemIt = renderedItems_.find(renderedItemIdIt.value());
    if (renderedItemIt == renderedItems_.end() || renderedItemIt.value() == nullptr) {
        return nullptr;
    }

    return static_cast<RenderedWeaveNodeItem*>(renderedItemIt.value().get());
}
