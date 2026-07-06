#include "Editor/Grid/WeaveEditorGrid.h"

#include <QPainter>

#include <algorithm>
#include <cmath>

void WeaveEditorGrid::Update(QSize viewport_size, CanvasCamera *camera) {
    cachedGridLines_.clear();

    if (
        camera == nullptr ||
        canvasGridSettings_.minorSpacing <= 0.0F ||
        viewport_size.width() <= 0 ||
        viewport_size.height() <= 0 ||
        camera->GetZoom() <= 0.0F
    ) {
        return;
    }

    const float viewportHalfWidth = static_cast<float>(viewport_size.width()) * 0.5F;
    const float viewportHalfHeight = static_cast<float>(viewport_size.height()) * 0.5F;
    const float worldUnitsPerPixel = 1.0F / camera->GetZoom();
    const Vector2D pan = camera->GetPan();
    const float worldLeft = pan.x - viewportHalfWidth * worldUnitsPerPixel;
    const float worldTop = pan.y - viewportHalfHeight * worldUnitsPerPixel;
    const float worldRight = pan.x + viewportHalfWidth * worldUnitsPerPixel;
    const float worldBottom = pan.y + viewportHalfHeight * worldUnitsPerPixel;

    const float firstWorldX =
        std::floor(worldLeft / canvasGridSettings_.minorSpacing) * canvasGridSettings_.minorSpacing;
    const float firstWorldY =
        std::floor(worldTop / canvasGridSettings_.minorSpacing) * canvasGridSettings_.minorSpacing;
    const int majorLineInterval = std::max(canvasGridSettings_.majorLineInterval, 1);

    const int verticalLineCount =
        static_cast<int>(std::ceil((worldRight - firstWorldX) / canvasGridSettings_.minorSpacing)) + 2;
    const int horizontalLineCount =
        static_cast<int>(std::ceil((worldBottom - firstWorldY) / canvasGridSettings_.minorSpacing)) + 2;
    cachedGridLines_.reserve(verticalLineCount + horizontalLineCount);

    for (int columnIndex = 0; columnIndex < verticalLineCount; ++columnIndex) {
        const float worldX =
            firstWorldX + static_cast<float>(columnIndex) * canvasGridSettings_.minorSpacing;
        const int worldColumnIndex =
            static_cast<int>(std::llround(worldX / canvasGridSettings_.minorSpacing));
        const float x = (worldX - pan.x) * camera->GetZoom() + viewportHalfWidth;
        cachedGridLines_.push_back({
            .start = {x, 0.0F},
            .end = {x, static_cast<float>(viewport_size.height())},
            .isMajor = worldColumnIndex % majorLineInterval == 0
        });
    }

    for (int rowIndex = 0; rowIndex < horizontalLineCount; ++rowIndex) {
        const float worldY =
            firstWorldY + static_cast<float>(rowIndex) * canvasGridSettings_.minorSpacing;
        const int worldRowIndex =
            static_cast<int>(std::llround(worldY / canvasGridSettings_.minorSpacing));
        const float y = (worldY - pan.y) * camera->GetZoom() + viewportHalfHeight;
        cachedGridLines_.push_back({
            .start = {0.0F, y},
            .end = {static_cast<float>(viewport_size.width()), y},
            .isMajor = worldRowIndex % majorLineInterval == 0
        });
    }
}

void WeaveEditorGrid::Paint(QPainter &painter) {
    for (const CanvasGridLine& gridLine : cachedGridLines_) {
        QPen pen(gridLine.isMajor ? kCanvasMajorGridColor : kCanvasMinorGridColor);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawLine(
            QPointF(gridLine.start.x, gridLine.start.y),
            QPointF(gridLine.end.x, gridLine.end.y)
        );
    }
}
