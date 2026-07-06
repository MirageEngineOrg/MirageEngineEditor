#pragma once
#include <QSizeF>
class CanvasCamera;
class QPainter;
class IGridPainter;
class ICanvasGrid {
public:
    virtual ~ICanvasGrid() = default;
    virtual void Update(QSize viewport_size, CanvasCamera* camera) = 0;
    virtual void Paint(QPainter& painter) = 0;
};
