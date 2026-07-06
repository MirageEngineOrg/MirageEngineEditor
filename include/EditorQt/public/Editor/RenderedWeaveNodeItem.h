#pragma once
#include "WaveGraph/WeaveGraphTypes.hpp"
#include "WeaveCanvasBase/RenderedItem/RenderedItem.h"

#include <QColor>
#include <QPointF>
#include <QPainter>
#include <QRectF>
#include <QString>
#include <QVector>

enum class WeavePinDirection {
     Input,
     Output
};

class CanvasCamera;

struct WeaveCanvasPinViewData {
    int pin_id;
    QString name;
    QColor color;
    WeavePinDirection direction {WeavePinDirection::Input};
    bool isActive {false};
};

class RenderedWeaveNodeItem : public IRenderedItem {
  public:

    RenderedWeaveNodeItem(
        const QString& title,
        const QString& description,
        const QVector<WeaveCanvasPinViewData>& pins,
        Vector2D position = Vector2D(0, 0));

    [[nodiscard]] RenderedItemID GetID() const override;
    [[nodiscard]] Vector2D GetSize() const override;
    [[nodiscard]] Vector2D GetPosition() const override;
    void SetPosition(const Vector2D& position) override;
    void SetRect(const QRectF& rect) override;
    void OnSelect() override;
    void OnUnselect() override;

    QRectF BuildNodeScreenRect(QSize viewportSize, CanvasCamera* camera) const;
    QRectF ScreenFromGlobalRect(QSize viewportSize, CanvasCamera* camera,const QRectF &global_rect) const;
    [[nodiscard]] int GetPinIdAtWorldPosition(
        const QPointF& worldPosition,
        float zoom
    ) const;
    [[nodiscard]] Vector2D GetPinWorldPosition(int pinId) const;
    [[nodiscard]] bool TryGetPinById(int pinId, WeaveCanvasPinViewData& outPin) const;
    [[nodiscard]] bool SetPinActive(int pinId, bool isActive);

    void Paint(QPainter &painter, QSize viewportSize, CanvasCamera *camera) const override;

private:
    RenderedItemID renderedItemId_ {RenderedItemIDGenerator::Next()};
    bool isSelected_ {false};
    QString title;
    QString description;
    QVector<WeaveCanvasPinViewData> pins;
    Vector2D position;
    Vector2D size;

    void CalculateSize();
};
