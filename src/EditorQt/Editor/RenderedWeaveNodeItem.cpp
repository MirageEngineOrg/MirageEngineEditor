#include "Editor/RenderedWeaveNodeItem.h"

#include "WeaveCanvasBase/CanvasCamera.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>
#include <cmath>

namespace {
const QColor kNodeBodyColor(35, 44, 56);
const QColor kNodeHeaderColor(48, 61, 78);
const QColor kNodeBorderColor(105, 122, 146);
const QColor kNodeTextColor(230, 235, 241);
const QColor kNodeSelectionOutlineColor(212, 175, 55);
constexpr float kNodeCornerRadius = 10.0F;
constexpr float kNodeHeaderHeight = 40.0F;
constexpr float kNodeTitlePaddingX = 12.0F;
constexpr float kNodeDescriptionPaddingX = 12.0F;
constexpr float kNodeDescriptionPaddingTop = 20.0F;
constexpr float kNodeDescriptionPaddingBottom = 10.0F;
constexpr float kNodePinsHorizontalPadding = 12.0F;
constexpr float kNodePinsVerticalPadding = 12.0F;
constexpr float kNodePinRadius = 7.0F;
constexpr float kNodePinTextSpacing = 8.0F;
constexpr float kNodeBaseBodyHeight = 100.0F;
constexpr float kNodePinRowHeight = 24.0F;
constexpr float kNodePinAreaTopPadding = 12.0F;
constexpr float kNodePinAreaBottomPadding = 12.0F;

WeavePinDirection DirectionFromBool(const bool inputPins) {
    return inputPins ? WeavePinDirection::Input : WeavePinDirection::Output;
}

bool PinMatchesDirection(const WeaveCanvasPinViewData& pin, const bool inputPins) {
    return pin.direction == DirectionFromBool(inputPins);
}

int CountPinsWithDirection(const QVector<WeaveCanvasPinViewData>& pins, const bool inputPins) {
    return static_cast<int>(std::count_if(
        pins.begin(),
        pins.end(),
        [inputPins](const WeaveCanvasPinViewData& pin) {
            return PinMatchesDirection(pin, inputPins);
        }
    ));
}

QPointF BuildPinCenterWorldPosition(
    const Vector2D& nodePosition,
    const Vector2D& nodeSize,
    const int pinRowIndex,
    const bool inputPins
) {
    const float nodeLeft = nodePosition.x - nodeSize.x * 0.5F;
    const float nodeTop = nodePosition.y - nodeSize.y * 0.5F;
    const float nodeRight = nodePosition.x + nodeSize.x * 0.5F;
    const float firstPinCenterY =
        nodeTop + kNodeHeaderHeight + kNodePinsVerticalPadding + kNodePinRowHeight * 0.5F;
    const float pinCenterX = inputPins
        ? nodeLeft + kNodePinsHorizontalPadding + kNodePinRadius
        : nodeRight - kNodePinsHorizontalPadding - kNodePinRadius;

    return QPointF(
        pinCenterX,
        firstPinCenterY + static_cast<float>(pinRowIndex) * kNodePinRowHeight
    );
}

void DrawNodeText(
    QPainter& painter,
    const QRect& textRect,
    const QString& text,
    const int pixelSize,
    const int alignment,
    const bool wordWrap
) {
    QFont font = painter.font();
    font.setPixelSize(pixelSize);
    font.setHintingPreference(QFont::PreferFullHinting);
    painter.setFont(font);
    painter.setPen(kNodeTextColor);

    int flags = alignment;
    if (wordWrap) {
        flags |= Qt::TextWordWrap;
    }

    painter.drawText(textRect, flags, text);
}

void DrawPin(
    QPainter& painter,
    const WeaveCanvasPinViewData& pin,
    const QPointF& pinCenter,
    const QRect& textRect,
    const int pixelSize,
    const int alignment,
    const float zoom
) {
    const float scaledPinRadius = std::max(2.0F, kNodePinRadius * zoom);
    const float outlineThickness = std::max(1.5F, zoom);

    painter.save();
    painter.setPen(QPen(pin.color, outlineThickness));
    painter.setBrush(pin.isActive ? QBrush(pin.color) : Qt::NoBrush);
    painter.drawEllipse(pinCenter, scaledPinRadius, scaledPinRadius);
    painter.restore();

    if (textRect.width() > 2 && textRect.height() > 2) {
        DrawNodeText(painter, textRect, pin.name, pixelSize, alignment, false);
    }
}

void DrawNodeBody(
    QPainter& painter,
    const QRectF& borderRect,
    const QRectF& nodeRect,
    const QRectF& headerRect,
    const float borderThickness,
    const float cornerRadius
) {
    QPainterPath borderPath;
    borderPath.addRoundedRect(borderRect, cornerRadius, cornerRadius);

    QPainterPath bodyPath;
    bodyPath.addRoundedRect(
        nodeRect,
        std::max(1.0F, cornerRadius - borderThickness),
        std::max(1.0F, cornerRadius - borderThickness)
    );

    painter.setPen(Qt::NoPen);
    painter.setBrush(kNodeBorderColor);
    painter.drawPath(borderPath);

    painter.setBrush(kNodeBodyColor);
    painter.drawPath(bodyPath);

    painter.setBrush(kNodeHeaderColor);
    painter.drawRoundedRect(headerRect, cornerRadius, cornerRadius);
    painter.drawRect(
        QRectF(
            headerRect.x(),
            headerRect.y() + headerRect.height() * 0.5F,
            headerRect.width(),
            headerRect.height() * 0.5F
        )
    );
}

void DrawSelectionOutline(
    QPainter& painter,
    const QRectF& nodeRect,
    const float cornerRadius,
    const float outlineThickness
) {
    const float inset = outlineThickness * 0.5F;
    const QRectF outlineRect = nodeRect.adjusted(inset, inset, -inset, -inset);

    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(kNodeSelectionOutlineColor, outlineThickness));
    painter.drawRoundedRect(
        outlineRect,
        std::max(1.0F, cornerRadius - inset),
        std::max(1.0F, cornerRadius - inset)
    );
    painter.restore();
}
} // namespace



RenderedWeaveNodeItem::RenderedWeaveNodeItem(
    const QString& title,
    const QString& description,
    const QVector<WeaveCanvasPinViewData>& pins,
    const Vector2D position
)
    : title(title)
    , description(description)
    , pins(pins)
    , position(position) {
    CalculateSize();
}

void RenderedWeaveNodeItem::OnSelect() {
    isSelected_ = true;
}
void RenderedWeaveNodeItem::OnUnselect() {
    isSelected_ = false;
}

RenderedItemID RenderedWeaveNodeItem::GetID() const {
    return renderedItemId_;
}

Vector2D RenderedWeaveNodeItem::GetSize() const {
    return size;
}

Vector2D RenderedWeaveNodeItem::GetPosition() const {
    return position;
}

void RenderedWeaveNodeItem::SetPosition(const Vector2D& newPosition) {
    position = newPosition;
}

void RenderedWeaveNodeItem::SetRect(const QRectF& rect) {
    position = Vector2D {
        static_cast<float>(rect.center().x()),
        static_cast<float>(rect.center().y())
    };
    size = Vector2D {
        static_cast<float>(rect.width()),
        static_cast<float>(rect.height())
    };
}

QRectF RenderedWeaveNodeItem::BuildNodeScreenRect(QSize viewportSize, CanvasCamera* camera) const {
    return ScreenFromGlobalRect(
        viewportSize,
        camera,
        QRectF(position.x - size.x / 2, position.y - size.y / 2, size.x, size.y)
    );
}

QRectF RenderedWeaveNodeItem::ScreenFromGlobalRect(
    QSize viewportSize,
    CanvasCamera* camera,
    const QRectF& global_rect
) const {
    const float viewportHalfWidth = static_cast<float>(viewportSize.width()) * 0.5F;
    const float viewportHalfHeight = static_cast<float>(viewportSize.height()) * 0.5F;
    const Vector2D pan = camera->GetPan();
    const float zoom = camera->GetZoom();

    const float screenX = (global_rect.center().x() - pan.x) * zoom + viewportHalfWidth;
    const float screenY = (global_rect.center().y() - pan.y) * zoom + viewportHalfHeight;
    const float screenWidth = global_rect.size().width() * zoom;
    const float screenHeight = global_rect.size().height() * zoom;

    return QRectF(
        screenX - screenWidth / 2,
        screenY - screenHeight / 2,
        screenWidth,
        screenHeight
    );
}

Vector2D RenderedWeaveNodeItem::GetPinWorldPosition(const int pinId) const {
    if (pins.isEmpty()) {
        return {};
    }

    int inputRowIndex = 0;
    int outputRowIndex = 0;
    for (const WeaveCanvasPinViewData& pin : pins) {
        const bool isInputPin = pin.direction == WeavePinDirection::Input;
        const int pinRowIndex = isInputPin ? inputRowIndex++ : outputRowIndex++;

        if (pin.pin_id == pinId) {
            const QPointF pinCenter = BuildPinCenterWorldPosition(position, size, pinRowIndex, isInputPin);
            return Vector2D {
                static_cast<float>(pinCenter.x()),
                static_cast<float>(pinCenter.y())
            };
        }

    }

    return {};
}

bool RenderedWeaveNodeItem::TryGetPinById(const int pinId, WeaveCanvasPinViewData& outPin) const {
    for (const WeaveCanvasPinViewData& pin : pins) {
        if (pin.pin_id == pinId) {
            outPin = pin;
            return true;
        }
    }

    return false;
}

bool RenderedWeaveNodeItem::SetPinActive(const int pinId, const bool isActive) {
    for (WeaveCanvasPinViewData& pin : pins) {
        if (pin.pin_id == pinId) {
            pin.isActive = isActive;
            return true;
        }
    }

    return false;
}

int RenderedWeaveNodeItem::GetPinIdAtWorldPosition(
    const QPointF& worldPosition,
    const float zoom
) const {
    if (pins.isEmpty()) {
        return -1;
    }

    const float safeZoom = std::max(0.001F, zoom);
    const float worldPinRadius = std::max(kNodePinRadius, 2.0F / safeZoom);
    int inputRowIndex = 0;
    int outputRowIndex = 0;

    for (const WeaveCanvasPinViewData& pin : pins) {
        const bool isInputPin = pin.direction == WeavePinDirection::Input;
        const int pinRowIndex = isInputPin ? inputRowIndex++ : outputRowIndex++;
        const QPointF pinCenter = BuildPinCenterWorldPosition(position, size, pinRowIndex, isInputPin);
        const QPointF delta = worldPosition - pinCenter;
        const float distanceSquared = static_cast<float>(delta.x() * delta.x() + delta.y() * delta.y());
        if (distanceSquared <= worldPinRadius * worldPinRadius) {
            return pin.pin_id;
        }
    }

    return -1;
}

void RenderedWeaveNodeItem::CalculateSize() {
    const int maxPinCount = std::max(CountPinsWithDirection(pins, true), CountPinsWithDirection(pins, false));
    if (maxPinCount <= 0) {
        size = Vector2D {kNodeBaseBodyHeight, kNodeBaseBodyHeight};
        return;
    }

    const float pinAreaHeight =
        kNodePinAreaTopPadding +
        static_cast<float>(maxPinCount) * kNodePinRowHeight +
        kNodePinAreaBottomPadding;

    size = Vector2D {300.0F, kNodeBaseBodyHeight + pinAreaHeight};
}

void RenderedWeaveNodeItem::Paint(QPainter& painter, QSize viewportSize, CanvasCamera* camera) const {
    if (camera == nullptr) {
        return;
    }

    const QRectF borderRect = BuildNodeScreenRect(viewportSize, camera);
    const float zoomScale = camera->GetZoom();
    const float borderThickness = std::max(1.0F, zoomScale);
    const float scaledCornerRadius = std::max(2.0F, kNodeCornerRadius * zoomScale);
    const float scaledHeaderHeight = std::max(1.0F, kNodeHeaderHeight * zoomScale);
    const float scaledTitlePaddingX = std::max(1.0F, kNodeTitlePaddingX * zoomScale);
    const float scaledDescriptionPaddingX = std::max(1.0F, kNodeDescriptionPaddingX * zoomScale);
    const float scaledDescriptionPaddingTop = std::max(1.0F, kNodeDescriptionPaddingTop * zoomScale);
    const float scaledDescriptionPaddingBottom = std::max(1.0F, kNodeDescriptionPaddingBottom * zoomScale);
    const float scaledPinsHorizontalPadding = std::max(2.0F, kNodePinsHorizontalPadding * zoomScale);
    const float scaledPinsVerticalPadding = std::max(2.0F, kNodePinsVerticalPadding * zoomScale);
    const float scaledPinRadius = std::max(2.0F, kNodePinRadius * zoomScale);
    const float scaledPinTextSpacing = std::max(2.0F, kNodePinTextSpacing * zoomScale);
    const float scaledPinRowHeight = std::max(1.0F, kNodePinRowHeight * zoomScale);
    const int maxPinCount = std::max(CountPinsWithDirection(pins, true), CountPinsWithDirection(pins, false));

    const QRectF nodeRect(
        borderRect.x() + borderThickness,
        borderRect.y() + borderThickness,
        borderRect.width() - borderThickness * 2.0F,
        borderRect.height() - borderThickness * 2.0F
    );
    const QRectF headerRect(nodeRect.x(), nodeRect.y(), nodeRect.width(), scaledHeaderHeight);
    const QRectF titleRectF(
        headerRect.x() + scaledTitlePaddingX,
        headerRect.y(),
        headerRect.width() - scaledTitlePaddingX * 2.0F,
        headerRect.height()
    );
    const float firstPinCenterY =
        headerRect.bottom() + scaledPinsVerticalPadding + scaledPinRowHeight * 0.5F;
    const float pinsBottomY =
        headerRect.bottom() +
        scaledPinsVerticalPadding +
        static_cast<float>(maxPinCount) * scaledPinRowHeight;

    const QRectF descriptionRectF(
        nodeRect.x() + scaledDescriptionPaddingX,
        pinsBottomY + scaledDescriptionPaddingTop,
        nodeRect.width() - scaledDescriptionPaddingX * 2.0F,
        std::max(
            0.0,
            nodeRect.bottom() - pinsBottomY - scaledDescriptionPaddingTop - scaledDescriptionPaddingBottom
        )
    );
    const QRect titleRect = titleRectF.toAlignedRect();
    const QRect descriptionRect = descriptionRectF.toAlignedRect();

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    DrawNodeBody(painter, borderRect, nodeRect, headerRect, borderThickness, scaledCornerRadius);
    if (isSelected_) {
        DrawSelectionOutline(
            painter,
            nodeRect,
            std::max(1.0F, scaledCornerRadius - borderThickness),
            std::max(2.0F, zoomScale * 1.5F)
        );
    }

    const int pinTextPixelSize =
        std::max(1, static_cast<int>(std::lround(headerRect.height() * 0.5F)));

    int inputRowIndex = 0;
    int outputRowIndex = 0;
    for (const WeaveCanvasPinViewData& pin : pins) {
        const bool isInputPin = pin.direction == WeavePinDirection::Input;
        const int pinRowIndex = isInputPin ? inputRowIndex++ : outputRowIndex++;
        const QPointF pinCenter(
            isInputPin
                ? nodeRect.x() + scaledPinsHorizontalPadding + scaledPinRadius
                : nodeRect.right() - scaledPinsHorizontalPadding - scaledPinRadius,
            firstPinCenterY + static_cast<float>(pinRowIndex) * scaledPinRowHeight
        );

        const QRect textRect = isInputPin
            ? QRectF(
                pinCenter.x() + scaledPinRadius + scaledPinTextSpacing,
                pinCenter.y() - scaledPinRowHeight * 0.5F,
                nodeRect.width() * 0.5F - scaledPinsHorizontalPadding * 2.0F,
                scaledPinRowHeight
            ).toAlignedRect()
            : QRectF(
                nodeRect.x() + nodeRect.width() * 0.5F,
                pinCenter.y() - scaledPinRowHeight * 0.5F,
                nodeRect.width() * 0.5F - scaledPinsHorizontalPadding * 2.0F - scaledPinRadius - scaledPinTextSpacing,
                scaledPinRowHeight
            ).toAlignedRect();

        DrawPin(
            painter,
            pin,
            pinCenter,
            textRect,
            pinTextPixelSize,
            isInputPin ? (Qt::AlignVCenter | Qt::AlignLeft) : (Qt::AlignVCenter | Qt::AlignRight),
            zoomScale
        );
    }

    const int scaledTitlePixelSize =
        std::max(1, static_cast<int>(std::lround(scaledHeaderHeight * 0.6F)));
    if (titleRect.width() > 2 && titleRect.height() > 2) {
        QFont titleFont = painter.font();
        titleFont.setPixelSize(scaledTitlePixelSize);
        const QFontMetrics fontMetrics(titleFont);
        const QString elidedTitle = fontMetrics.elidedText(title, Qt::ElideRight, titleRect.width());
        DrawNodeText(
            painter,
            titleRect,
            elidedTitle,
            scaledTitlePixelSize,
            Qt::AlignVCenter | Qt::AlignLeft,
            false
        );
    }

    if (!description.isEmpty() && descriptionRect.width() > 2 && descriptionRect.height() > 2) {
        DrawNodeText(
            painter,
            descriptionRect,
            description,
            std::max(1, static_cast<int>(std::lround(scaledHeaderHeight * 0.4F))),
            Qt::AlignLeft | Qt::AlignBottom,
            true
        );
    }

    painter.restore();
}
