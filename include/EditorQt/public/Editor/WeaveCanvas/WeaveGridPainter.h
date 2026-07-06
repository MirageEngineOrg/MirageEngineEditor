#pragma once

#include <QSize>
#include <QVector>

class QPainter;

struct CanvasCamera;
struct CanvasGridLine;
struct CanvasGridSettings;

struct WeaveGridPaintParams {
    QSize viewportSize {};
};

struct WeaveGridBuildParams {
    QSize viewportSize {};
    CanvasCamera const* camera {nullptr};
    CanvasGridSettings const* gridSettings {nullptr};
};

class WeaveGridPainter final {
public:
    [[nodiscard]] static QVector<CanvasGridLine> BuildGridLines(const WeaveGridBuildParams& params);
    static void DrawGrid(
        QPainter& painter,
        const QVector<CanvasGridLine>& gridLines,
        const WeaveGridPaintParams& params
    );
};
