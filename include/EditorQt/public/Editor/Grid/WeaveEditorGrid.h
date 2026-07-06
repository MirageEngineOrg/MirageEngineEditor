#pragma once
#include "Base/Vector2D.h"
#include "WeaveCanvasBase/CanvasCamera.h"
#include "WeaveCanvasBase/Grid/ICanvasGrid.h"

#include <QColor>
#include <QVector>

class QPainter;

struct CanvasGridLine {
    Vector2D start {};
    Vector2D end {};
    bool isMajor {false};
};

struct CanvasGridSettings {
    float minorSpacing {64.0F};
    int majorLineInterval {4};
};

class WeaveEditorGrid : public ICanvasGrid{
public:
    void Update(QSize viewport_size, CanvasCamera* camera) override;
    void Paint(QPainter& painter) override;
protected:
    QColor kCanvasMinorGridColor {52, 63, 79};
    QColor kCanvasMajorGridColor {78, 95, 118};
private:
    QVector<CanvasGridLine> cachedGridLines_;
    CanvasGridSettings canvasGridSettings_;
};
