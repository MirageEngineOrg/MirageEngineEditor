#include "WeaveCanvasBase/WeaveCanvasBase.h"
#include "WeaveCanvasBase/CanvasBaseInputController.h"
#include "WeaveCanvasBase/CanvasCamera.h"
#include "WeaveCanvasBase/Grid/ICanvasGrid.h"

#include <algorithm>
#include <cmath>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QStringList>

constexpr float kClickSelectionThreshold = 4.0F;

IBaseWeaveCanvas::IBaseWeaveCanvas(QWidget *parent,
                                    std::unique_ptr<ICanvasGrid> canvas_grid,
                                       std::unique_ptr<CanvasCamera> camera,
                                       CanvasBaseInputController* input_controller,
                                       const DefaultColorSettings& default_color_settings
                                       ) : QOpenGLWidget(parent) {

    this->canvasGrid_ = std::move(canvas_grid);
    this->inputController_ = input_controller;
    this->camera_ = std::move(camera);
    this->quadTree_ = std::make_unique<WeaveQuadTree>();
    initialize_opengl_surface();
    initialize_input_controller();

    this->currentRenderContext_ = {
        .defaultColorSettings = default_color_settings,
        .shouldRepaintCanvas = true
    };
}
void IBaseWeaveCanvas::AddRenderedItem(std::shared_ptr<IRenderedItem> item) {
    this->renderedItems_[item->GetID()] = item;
    if (quadTree_ != nullptr) {
         quadTree_->Insert(item.get());
    }
    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}
void IBaseWeaveCanvas::RemoveRenderedItem(RenderedItemID rendered_item_id) {
    if (!this->renderedItems_.contains(rendered_item_id)) return;
    this->renderedItems_.remove(rendered_item_id);
}

void IBaseWeaveCanvas::ApplyRenderedItemRect(const RenderedItemID renderedItemID, const QRectF& worldRect) {
    const auto it = renderedItems_.find(renderedItemID);
    if (it == renderedItems_.end() || it.value() == nullptr) {
        return;
    }

    it.value()->SetRect(worldRect);

    if (quadTree_ != nullptr) {
        const bool reindexSucceeded = quadTree_->Reindex(it.value().get());
        if (!reindexSucceeded) {
            QVector<IRenderedItem*> items;
            items.reserve(renderedItems_.size());

            for (const auto& renderedItem : renderedItems_) {
                if (renderedItem != nullptr) {
                    items.push_back(renderedItem.get());
                }
            }

            quadTree_->Rebuild(items);
        }
    }

    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}

std::optional<RenderedItemsMovementSession> IBaseWeaveCanvas::TakeCompletedMovementSession() {
    if (completedMovementSession_.IsEmpty()) {
        return std::nullopt;
    }

    std::optional<RenderedItemsMovementSession> session {std::move(completedMovementSession_)};
    completedMovementSession_ = {};
    return session;
}

IBaseWeaveCanvas::~IBaseWeaveCanvas() = default;

void IBaseWeaveCanvas::initialize_opengl_surface() {
    QSurfaceFormat surfaceFormat = format();
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setStencilBufferSize(8);
    surfaceFormat.setSamples(8);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(surfaceFormat);
    setAutoFillBackground(false);
}
void IBaseWeaveCanvas::initialize_input_controller() {
    inputController_->setParent(this);

    connect(inputController_, &CanvasBaseInputController::PanDeltaRequested, this, &IBaseWeaveCanvas::move);
    connect(inputController_, &CanvasBaseInputController::ZoomRequested, this, &IBaseWeaveCanvas::zoom);
    connect(inputController_, &CanvasBaseInputController::OnLeftMousePressed, this, &IBaseWeaveCanvas::OnLeftMousePressed);
    connect(inputController_, &CanvasBaseInputController::OnDrag, this, &IBaseWeaveCanvas::OnDrag);
    connect(inputController_, &CanvasBaseInputController::OnLeftMouseReleased, this, &IBaseWeaveCanvas::OnLeftMouseReleased);
}

void IBaseWeaveCanvas::move(float deltaX, float deltaY) {
    camera_->Move(deltaX, deltaY);
    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}

void IBaseWeaveCanvas::zoom(float delta) {
    camera_->Zoom(delta);
    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}

void IBaseWeaveCanvas::initializeGL() {
    initializeOpenGLFunctions();
}
void IBaseWeaveCanvas::paintGL() {
    QOpenGLWidget::paintGL();
    glViewport(0, 0, width(), height());
    glClearColor(static_cast<float>(currentRenderContext_.defaultColorSettings.kCanvasBackgroundColor.redF()),
                 static_cast<float>(currentRenderContext_.defaultColorSettings.kCanvasBackgroundColor.greenF()),
                 static_cast<float>(currentRenderContext_.defaultColorSettings.kCanvasBackgroundColor.blueF()),
                 static_cast<float>(currentRenderContext_.defaultColorSettings.kCanvasBackgroundColor.alphaF()));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    PaintCanvas(painter);
}

void IBaseWeaveCanvas::PaintCanvas(QPainter &painter) {
    if (currentRenderContext_.shouldRepaintCanvas) {
        canvasGrid_->Update(size(), camera_.get());
        currentRenderContext_.shouldRepaintCanvas = false;
    }

    canvasGrid_->Paint(painter);

    for (const auto& renderedItem : renderedItems_) {
        if (renderedItem != nullptr) {
            renderedItem->Paint(painter, size(), camera_.get());
        }
    }

    if (currentRenderContext_.shouldPaintSelectingArea_) {
        PaintSelectingBox(painter);
    }
}
void IBaseWeaveCanvas::Update() {
    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}

void IBaseWeaveCanvas::OnLeftMousePressed(float posX, float posY, bool controlPressed) {

    interactionContext_.pressScreenPosition_ = QPointF(posX, posY);
    interactionContext_.dragLastScreenPosition_ = interactionContext_.pressScreenPosition_;
    interactionContext_.pressControlPressed_ = controlPressed;
    interactionContext_.pressWasOnItem_ = false;
    interactionContext_.pressWasOnSelectedItem_ = false;
    pressedRenderedItemId_ = 0;

    const Vector2D pressGlobalPosition = global_from_screen(posX, posY);

    IRenderedItem* pressedItem =
        ResolvePressedRenderedItem(QPointF(pressGlobalPosition.x, pressGlobalPosition.y));

    interactionContext_.interactionMode_ = onPressedItem(
        {.posX = posX, .posY = posY, .bWasControlPressed = controlPressed}, pressedItem);
}

CanvasInteractionMode IBaseWeaveCanvas::onPressedItem(OnPressItemContext on_press_item_context,
                                                      IRenderedItem *item) {
    if (item == nullptr) {
        return CanvasInteractionMode::Idle;
    }

    pressedRenderedItemId_ = item->GetID();
    if (!on_press_item_context.bWasControlPressed) {
        ClearSelection();
    }
    SelectRenderedItem(pressedRenderedItemId_);
    HandleSelectionChanged();

    return  CanvasInteractionMode::PendingMoveSelection;
}

void IBaseWeaveCanvas::OnDrag(float mouseX, float mouseY) {
    if (interactionContext_.interactionMode_ == CanvasInteractionMode::PendingMoveSelection ||
        interactionContext_.interactionMode_ == CanvasInteractionMode::DraggingSelection) {
        const float deltaX = mouseX - static_cast<float>(interactionContext_.pressScreenPosition_.x());
        const float deltaY = mouseY - static_cast<float>(interactionContext_.pressScreenPosition_.x());

        if (std::abs(deltaX) > kClickSelectionThreshold || std::abs(deltaY) > kClickSelectionThreshold) {
            if (interactionContext_.interactionMode_ == CanvasInteractionMode::PendingMoveSelection && interactionContext_.pressWasOnItem_) {
                OnItemsDragStarted();
            }

            interactionContext_.interactionMode_ = CanvasInteractionMode::DraggingSelection;
            currentRenderContext_.shouldPaintSelectingArea_ = false;

            const float incrementalDeltaX = mouseX - static_cast<float>(interactionContext_.dragLastScreenPosition_.x());
            const float incrementalDeltaY = mouseY - static_cast<float>(interactionContext_.dragLastScreenPosition_.y());
            constexpr float kCanvasMinZoom = 0.1F;
            const float safeZoom = std::max(camera_->GetZoom(), kCanvasMinZoom);
            dragTickWorldDelta_ = Vector2D {
                incrementalDeltaX / safeZoom,
                incrementalDeltaY / safeZoom
            };

            OnItemsDragTick();
            interactionContext_.dragLastScreenPosition_ = QPointF(mouseX, mouseY);
        }
        return;
    }

    if (IsClickSelection(interactionContext_.pressScreenPosition_.x(), interactionContext_.pressScreenPosition_.y(), mouseX, mouseY)) {
        currentRenderContext_.shouldPaintSelectingArea_ = false;
        update();
        return;
    }

    const Vector2D start_global_pos = global_from_screen(interactionContext_.pressScreenPosition_.x(), interactionContext_.pressScreenPosition_.y());
    const Vector2D end_global_pos = global_from_screen(mouseX, mouseY);

    RecalculateSelectingArea(start_global_pos, end_global_pos);
    currentRenderContext_.shouldPaintSelectingArea_ = true;
    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}
void IBaseWeaveCanvas::OnLeftMouseReleased(float mouseX, float mouseY) {
    if (interactionContext_.interactionMode_ == CanvasInteractionMode::DraggingSelection) {
        interactionContext_.interactionMode_ = CanvasInteractionMode::Idle;
        currentRenderContext_.shouldPaintSelectingArea_ = false;
        currentRenderContext_.shouldRepaintCanvas = true;
        OnItemsDragEnded();
        if (!completedMovementSession_.IsEmpty()) {
            emit MovementSessionFinished();
        }
        update();
        return;
    }

    if (interactionContext_.interactionMode_ == CanvasInteractionMode::PendingMoveSelection) {
        interactionContext_.interactionMode_ = CanvasInteractionMode::Idle;
        currentRenderContext_.shouldPaintSelectingArea_ = false;
        currentRenderContext_.shouldRepaintCanvas = true;

        update();
        return;
    }

    const QVector<RenderedItemID> previousSelectedItems = selectedRenderedItemIds_;
    const Vector2D start_global_pos = global_from_screen(mouseX, mouseY);
    const Vector2D end_global_pos = global_from_screen(interactionContext_.pressScreenPosition_.x(), interactionContext_.pressScreenPosition_.y());

    currentRenderContext_.shouldPaintSelectingArea_ = false;
    currentRenderContext_.shouldRepaintCanvas = true;
    interactionContext_.interactionMode_ = CanvasInteractionMode::Idle;
    RecalculateSelectingArea(start_global_pos, end_global_pos);

    if (IsClickSelection(interactionContext_.pressScreenPosition_.x(), interactionContext_.pressScreenPosition_.y(),mouseX, mouseY)) {
        HandleRenderedItemClickSelection(
            quadTree_ != nullptr
                ? quadTree_->HitScan(QPointF(start_global_pos.x, start_global_pos.y))
                : nullptr,
            interactionContext_.pressControlPressed_
        );
    } else {
        HandleRectSelection(interactionContext_.pressControlPressed_);
    }

    QVector<RenderedItemID> newlySelectedItems;
    QVector<RenderedItemID> newlyUnselectedItems;

    for (const RenderedItemID itemId : selectedRenderedItemIds_) {
        if (!previousSelectedItems.contains(itemId)) {
            newlySelectedItems.push_back(itemId);
        }
    }

    for (const RenderedItemID itemId : previousSelectedItems) {
        if (!selectedRenderedItemIdLookup_.contains(itemId)) {
            newlyUnselectedItems.push_back(itemId);
        }
    }

    if (!newlySelectedItems.isEmpty()) {
        OnItemsSelected(newlySelectedItems);
    }

    if (!newlyUnselectedItems.isEmpty()) {
        OnItemsUnselected(newlyUnselectedItems);
    }

    HandleSelectionChanged();
    update();
}

// Input proxy to input controller
void IBaseWeaveCanvas::mousePressEvent(QMouseEvent *event) {
    inputController_->mousePressEvent(event);
    if (event->isAccepted()) {
        return;
    }
    QOpenGLWidget::mousePressEvent(event);
}

void IBaseWeaveCanvas::mouseMoveEvent(QMouseEvent *event) {
    inputController_->mouseMoveEvent(event);
    if (event->isAccepted()) {
        return;
    }
    QOpenGLWidget::mouseMoveEvent(event);
}

void IBaseWeaveCanvas::mouseReleaseEvent(QMouseEvent *event) {
    inputController_->mouseReleaseEvent(event);
    if (event->isAccepted()) {
        return;
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}

void IBaseWeaveCanvas::resizeEvent(QResizeEvent *event) {
    currentRenderContext_.shouldRepaintCanvas = true;
    QOpenGLWidget::resizeEvent(event);
}

void IBaseWeaveCanvas::wheelEvent(QWheelEvent *event) {
    inputController_->wheelEvent(event);
    if (event->isAccepted()) {
        return;
    }
    QOpenGLWidget::wheelEvent(event);
}

// Input proxy to input controller

void IBaseWeaveCanvas::PaintSelectingBox(QPainter& painter) {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QColor(128, 128, 128, 128));
    painter.setBrush(QColor(128, 128, 128, 128));
    painter.drawRect(ScreenFromGlobalRect(currentRenderContext_.selecting_global_area_));
    painter.restore();
}

void IBaseWeaveCanvas::RecalculateSelectingArea(const Vector2D& start, const Vector2D& end) {
    QPoint top_left {};
    QPoint bottom_right {};

    if (start.x <= end.x && start.y <= end.y) {
        top_left = QPoint(static_cast<int>(start.x), static_cast<int>(start.y));
        bottom_right = QPoint(static_cast<int>(end.x), static_cast<int>(end.y));
    } else if (start.x >= end.x && start.y >= end.y) {
        top_left = QPoint(static_cast<int>(end.x), static_cast<int>(end.y));
        bottom_right = QPoint(static_cast<int>(start.x), static_cast<int>(start.y));
    } else if (start.x >= end.x && start.y <= end.y) {
        top_left = QPoint(static_cast<int>(end.x), static_cast<int>(start.y));
        bottom_right = QPoint(static_cast<int>(start.x), static_cast<int>(end.y));
    } else {
        bottom_right = QPoint(static_cast<int>(end.x), static_cast<int>(start.y));
        top_left = QPoint(static_cast<int>(start.x), static_cast<int>(end.y));
    }

    currentRenderContext_.selecting_global_area_ = QRect(top_left, bottom_right);
}

Vector2D IBaseWeaveCanvas::global_from_screen(const float posX, const float posY) const {
    constexpr float kCanvasMinZoom = 0.1F;
    const float safeZoom = std::max(camera_->GetZoom(), kCanvasMinZoom);
    const float viewportHalfWidth = static_cast<float>(width()) * 0.5F;
    const float viewportHalfHeight = static_cast<float>(height()) * 0.5F;
    const Vector2D pan = camera_->GetPan();

    return Vector2D {
        ((posX - viewportHalfWidth) / safeZoom) + pan.x,
        ((posY - viewportHalfHeight) / safeZoom) + pan.y
    };
}

QRectF IBaseWeaveCanvas::ScreenFromGlobalRect(const QRectF& global_rect) const {
    const float viewportHalfWidth = static_cast<float>(width()) * 0.5F;
    const float viewportHalfHeight = static_cast<float>(height()) * 0.5F;
    const Vector2D pan = camera_->GetPan();
    const float zoom = camera_->GetZoom();

    const float screenX = (global_rect.center().x() - pan.x) * zoom + viewportHalfWidth;
    const float screenY = (global_rect.center().y() - pan.y) * zoom + viewportHalfHeight;
    const float screenWidth = global_rect.size().width() * zoom;
    const float screenHeight = global_rect.size().height() * zoom;

    return QRectF(
        screenX - screenWidth / 2.0F,
        screenY - screenHeight / 2.0F,
        screenWidth,
        screenHeight
    );
}

bool IBaseWeaveCanvas::IsClickSelection(
    const float startX,
    const float startY,
    const float endX,
    const float endY
) const {
    return std::abs(endX - startX) <= kClickSelectionThreshold &&
           std::abs(endY - startY) <= kClickSelectionThreshold;
}

void IBaseWeaveCanvas::HandleRenderedItemClickSelection(
    const IRenderedItem* clickedItem,
    const bool controlPressed
) {
    if (clickedItem == nullptr) {
        if (!controlPressed) {
            ClearSelection();
        }
        return;
    }

    const RenderedItemID itemId = clickedItem->GetID();
    const bool wasSelected = IsRenderedItemSelected(itemId);

    if (controlPressed) {
        if (wasSelected) {
            DeselectRenderedItem(itemId);
        } else {
            SelectRenderedItem(itemId);
        }
        return;
    }

    ClearSelection();
    SelectRenderedItem(itemId);
}

void IBaseWeaveCanvas::HandleRectSelection(const bool controlPressed) {
    if (quadTree_ == nullptr) {
        if (!controlPressed) {
            ClearSelection();
        }
        return;
    }

    const QVector<const IRenderedItem*> selectedItems = quadTree_->Query(currentRenderContext_.selecting_global_area_);

    if (!controlPressed) {
        ClearSelection();
    }

    for (const IRenderedItem* selectedItem : selectedItems) {
        if (selectedItem == nullptr) {
            continue;
        }

        const RenderedItemID itemId = selectedItem->GetID();
        if (controlPressed && IsRenderedItemSelected(itemId)) {
            DeselectRenderedItem(itemId);
        } else {
            SelectRenderedItem(itemId);
        }
    }
}

void IBaseWeaveCanvas::ClearSelection() {
    OnItemsUnselected(selectedRenderedItemIds_);
    selectedRenderedItemIds_.clear();
    selectedRenderedItemIdLookup_.clear();
}

void IBaseWeaveCanvas::SelectRenderedItem(const RenderedItemID renderedItemID) {
    if (selectedRenderedItemIdLookup_.contains(renderedItemID)) {
        return;
    }
    renderedItems_[renderedItemID]->OnSelect();
    selectedRenderedItemIds_.push_back(renderedItemID);
    selectedRenderedItemIdLookup_.insert(renderedItemID);
}

void IBaseWeaveCanvas::DeselectRenderedItem(const RenderedItemID renderedItemID) {
    if (!selectedRenderedItemIdLookup_.remove(renderedItemID)) {
        return;
    }

    const auto it = std::find(selectedRenderedItemIds_.begin(), selectedRenderedItemIds_.end(), renderedItemID);
    if (it != selectedRenderedItemIds_.end()) {
        selectedRenderedItemIds_.erase(it);
        renderedItems_[*it]->OnUnselect();
    }
}

bool IBaseWeaveCanvas::IsRenderedItemSelected(const RenderedItemID renderedItemID) const {
    return selectedRenderedItemIdLookup_.contains(renderedItemID);
}

QVector<RenderedItemID> IBaseWeaveCanvas::GetSelectedRenderedItemIds() const {
    return selectedRenderedItemIds_;
}

void IBaseWeaveCanvas::HandleSelectionChanged() {
    QStringList selectedIds;
    selectedIds.reserve(selectedRenderedItemIds_.size());

    for (const RenderedItemID itemId : selectedRenderedItemIds_) {
        selectedIds.push_back(QString::number(itemId));
    }

    qDebug() << "Selected rendered items:" << selectedIds.join(", ");
}

void IBaseWeaveCanvas::OnItemsSelected(const QVector<RenderedItemID>& selectedItems) {
    for (auto itemId : selectedItems) {
        renderedItems_[itemId]->OnSelect();
    }
}

void IBaseWeaveCanvas::OnItemsUnselected(const QVector<RenderedItemID>& unselectedItems) {
    for (auto itemId : unselectedItems) {
        renderedItems_[itemId]->OnUnselect();
    }
}

void IBaseWeaveCanvas::OnItemsDragStarted() {
    activeMovementSession_.itemGeometryChanges.clear();
    activeMovementSession_.itemGeometryChanges.reserve(selectedRenderedItemIds_.size());

    for (const RenderedItemID itemId : selectedRenderedItemIds_) {
        const auto it = renderedItems_.find(itemId);
        if (it == renderedItems_.end() || it.value() == nullptr) {
            continue;
        }

        const QRectF itemRect = BuildRenderedItemWorldRect(it.value().get());
        activeMovementSession_.itemGeometryChanges.push_back({
            .renderedItemID = itemId,
            .beforeRect = itemRect,
            .afterRect = itemRect,
        });
    }
}

void IBaseWeaveCanvas::OnItemsDragTick() {
    for (const RenderedItemID itemId : selectedRenderedItemIds_) {
        const auto it = renderedItems_.find(itemId);
        if (it == renderedItems_.end() || it.value() == nullptr) {
            continue;
        }

        Vector2D position = it.value()->GetPosition();
        position.x += dragTickWorldDelta_.x;
        position.y += dragTickWorldDelta_.y;
        it.value()->SetPosition(position);
    }

    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}

void IBaseWeaveCanvas::OnItemsDragEnded() {
    if (quadTree_ != nullptr) {
        bool shouldFallbackToRebuild = false;

        for (const RenderedItemID itemId : selectedRenderedItemIds_) {
            const auto it = renderedItems_.find(itemId);
            if (it == renderedItems_.end() || it.value() == nullptr) {
                continue;
            }

            if (!quadTree_->Reindex(it.value().get())) {
                shouldFallbackToRebuild = true;
                break;
            }
        }

        if (shouldFallbackToRebuild) {
            QVector<IRenderedItem*> items;
            items.reserve(renderedItems_.size());

            for (const auto& renderedItem : renderedItems_) {
                if (renderedItem != nullptr) {
                    items.push_back(renderedItem.get());
                }
            }

            quadTree_->Rebuild(items);
        }
    }

    for (RenderedItemGeometryChange& geometryChange : activeMovementSession_.itemGeometryChanges) {
        const auto it = renderedItems_.find(geometryChange.renderedItemID);
        if (it == renderedItems_.end() || it.value() == nullptr) {
            continue;
        }

        geometryChange.afterRect = BuildRenderedItemWorldRect(it.value().get());
    }

    completedMovementSession_ = activeMovementSession_;
    activeMovementSession_.itemGeometryChanges.clear();
    currentRenderContext_.shouldRepaintCanvas = true;
    update();
}

IRenderedItem *IBaseWeaveCanvas::ResolvePressedRenderedItem(const QPointF &worldPosition) const {
    if (quadTree_ == nullptr) {
        return nullptr;
    }

    return quadTree_->HitScan(worldPosition);
}

QRectF IBaseWeaveCanvas::BuildRenderedItemWorldRect(const IRenderedItem* renderedItem) const {
    if (renderedItem == nullptr) {
        return {};
    }

    const Vector2D position = renderedItem->GetPosition();
    const Vector2D size = renderedItem->GetSize();

    return QRectF(
        static_cast<qreal>(position.x - size.x * 0.5F),
        static_cast<qreal>(position.y - size.y * 0.5F),
        static_cast<qreal>(size.x),
        static_cast<qreal>(size.y)
    );
}
