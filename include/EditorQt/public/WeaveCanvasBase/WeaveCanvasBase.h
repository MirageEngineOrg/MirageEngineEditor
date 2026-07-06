#pragma once
#include "CanvasBaseInputController.h"
#include "CanvasCamera.h"
#include "RenderedItem/RenderedItem.h"
#include "WeaveQuadTree.h"

#include <QHash>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QRect>
#include <QSet>
#include <QVector>

#include <optional>

class CanvasCamera;
class CanvasBaseInputController;
class ICanvasGrid;

struct DefaultColorSettings {
    QColor kCanvasBackgroundColor;
};

inline DefaultColorSettings init_default_color_settings = {.kCanvasBackgroundColor = QColor(22, 28, 36, 1.f) };

struct CurrentRenderContext {
    DefaultColorSettings defaultColorSettings;
    bool shouldRepaintCanvas = false;

    QRect selecting_global_area_ {};
    bool shouldPaintSelectingArea_ {false};
};

enum class CanvasInteractionMode {
    Idle,
    SelectingRect,
    PendingMoveSelection,
    DraggingSelection
};

struct RenderedItemGeometryChange {
    RenderedItemID renderedItemID {0};
    QRectF beforeRect {};
    QRectF afterRect {};
};

struct RenderedItemsMovementSession {
    QVector<RenderedItemGeometryChange> itemGeometryChanges {};

    [[nodiscard]] bool IsEmpty() const noexcept {
        return itemGeometryChanges.isEmpty();
    }
};

struct OnPressItemContext {
    float posX;
    float posY;
    bool bWasControlPressed = false;
};


struct InteractionContext {
    CanvasInteractionMode interactionMode_ {CanvasInteractionMode::Idle};

    QPointF dragLastScreenPosition_ {};
    QPointF pressScreenPosition_ {};
    bool pressControlPressed_ {false};
    bool pressWasOnItem_ {false};
    bool pressWasOnSelectedItem_ {false};
};

class IBaseWeaveCanvas : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    ~IBaseWeaveCanvas() override;
    explicit IBaseWeaveCanvas(
        QWidget* parent,
        std::unique_ptr<ICanvasGrid>,
        std::unique_ptr<CanvasCamera> = std::make_unique<CanvasCamera>(),
        CanvasBaseInputController* = new CanvasBaseInputController(),
        const DefaultColorSettings& default_color_settings = init_default_color_settings
        );

    void AddRenderedItem(std::shared_ptr<IRenderedItem> item);
    void RemoveRenderedItem(RenderedItemID rendered_item_id);
    void ApplyRenderedItemRect(RenderedItemID renderedItemID, const QRectF& worldRect);
    [[nodiscard]] std::optional<RenderedItemsMovementSession> TakeCompletedMovementSession();
signals:
    void MovementSessionFinished();
protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void OnLeftMousePressed(float posX, float posY, bool controlPressed);
    virtual CanvasInteractionMode onPressedItem(OnPressItemContext on_press_item_context,
                                                IRenderedItem *item);

    virtual void OnDrag(float mouseX, float mouseY);
    virtual void OnLeftMouseReleased(float mouseX, float mouseY);
    [[nodiscard]] QVector<RenderedItemID> GetSelectedRenderedItemIds() const;
    virtual void HandleSelectionChanged();
    virtual void OnItemsSelected(const QVector<RenderedItemID>& selectedItems);
    virtual void OnItemsUnselected(const QVector<RenderedItemID>& unselectedItems);
    virtual void OnItemsDragStarted();
    virtual void OnItemsDragTick();
    virtual void OnItemsDragEnded();
    [[nodiscard]] IRenderedItem *ResolvePressedRenderedItem(const QPointF &worldPosition) const;

    QHash<RenderedItemID, std::shared_ptr<IRenderedItem>> renderedItems_;

    std::unique_ptr<CanvasCamera> camera_;


    virtual void PaintCanvas(QPainter &painter);
    void Update();

    void PaintSelectingBox(QPainter& painter);
    void RecalculateSelectingArea(const Vector2D& start, const Vector2D& end);
    [[nodiscard]] Vector2D global_from_screen(float posX, float posY) const;
    [[nodiscard]] QRectF ScreenFromGlobalRect(const QRectF& global_rect) const;
    [[nodiscard]] QRectF BuildRenderedItemWorldRect(const IRenderedItem* renderedItem) const;
    [[nodiscard]] bool IsClickSelection(float startX, float startY, float endX, float endY) const;
    void HandleRenderedItemClickSelection(const IRenderedItem* clickedItem, bool controlPressed);
    void HandleRectSelection(bool controlPressed);
    void ClearSelection();
    void SelectRenderedItem(RenderedItemID renderedItemID);
    void DeselectRenderedItem(RenderedItemID renderedItemID);
    [[nodiscard]] bool IsRenderedItemSelected(RenderedItemID renderedItemID) const;
private:
    void initialize_opengl_surface();
    void initialize_input_controller();

    void move(float deltaX, float deltaY);
    void zoom(float delta);

    std::unique_ptr<ICanvasGrid> canvasGrid_;
    std::unique_ptr<WeaveQuadTree> quadTree_;
    CanvasBaseInputController* inputController_;

    CurrentRenderContext currentRenderContext_;
    InteractionContext interactionContext_;

    RenderedItemID pressedRenderedItemId_ {0};
    QVector<RenderedItemID> selectedRenderedItemIds_ {};
    QSet<RenderedItemID> selectedRenderedItemIdLookup_ {};
    Vector2D dragTickWorldDelta_ {};
    RenderedItemsMovementSession activeMovementSession_ {};
    RenderedItemsMovementSession completedMovementSession_ {};
};
