#pragma once

#include <QRectF>

class QPainter;

struct WeaveCanvasNodeViewData;

struct WeaveNodePaintParams {
    QRectF borderRect {};
    float zoom {1.0F};
};

class WeaveNodePainter final {
public:
    [[nodiscard]] static float CalculateNodeBodyHeight(const WeaveCanvasNodeViewData& node);
    static void DrawNode(
        QPainter& painter,
        const WeaveCanvasNodeViewData& node,
        const WeaveNodePaintParams& params
    );
};
