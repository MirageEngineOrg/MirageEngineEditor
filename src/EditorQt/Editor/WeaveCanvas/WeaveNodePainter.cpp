// // #include "Editor/WeaveCanvas/WeaveNodePainter.h"
// //
// // #include "Editor/WeaveCanvas/WeaveCanvas.h"
// //
// // #include <algorithm>
// // #include <cmath>
// // #include <QColor>
// // #include <QFont>
// // #include <QFontMetrics>
// // #include <QPainter>
// // #include <QPainterPath>
// // #include <QRect>
// //
// // namespace {
// // const QColor kNodeBodyColor(35, 44, 56);
// // const QColor kNodeHeaderColor(48, 61, 78);
// // const QColor kNodeBorderColor(105, 122, 146);
// // const QColor kNodeTextColor(230, 235, 241);
// // constexpr float kNodeCornerRadius = 10.0F;
// // constexpr float kNodeHeaderHeight = 40.0F;
// // constexpr float kNodeTitlePaddingX = 12.0F;
// // constexpr float kNodeDescriptionPaddingX = 12.0F;
// // constexpr float kNodeDescriptionPaddingTop = 20.0F;
// // constexpr float kNodeDescriptionPaddingBottom = 10.0F;
// // constexpr float kNodePinsHorizontalPadding = 12.0F;
// // constexpr float kNodePinsVerticalPadding = 12.0F;
// // constexpr float kNodePinRadius = 7.0F;
// // constexpr float kNodePinTextSpacing = 8.0F;
// // constexpr float kNodeBaseBodyHeight = 100.0F;
// // constexpr float kNodePinRowHeight = 24.0F;
// // constexpr float kNodePinAreaTopPadding = 12.0F;
// // constexpr float kNodePinAreaBottomPadding = 12.0F;
// //
// // void DrawNodeText(
// //     QPainter& painter,
// //     const QRect& textRect,
// //     const QString& text,
// //     const int pixelSize,
// //     const int alignment,
// //     const bool wordWrap
// // ) {
// //     QFont font = painter.font();
// //     font.setPixelSize(pixelSize);
// //     font.setHintingPreference(QFont::PreferFullHinting);
// //     painter.setFont(font);
// //     painter.setPen(kNodeTextColor);
// //
// //     int flags = alignment;
// //     if (wordWrap) {
// //         flags |= Qt::TextWordWrap;
// //     }
// //
// //     painter.drawText(textRect, flags, text);
// // }
// //
// // void DrawPin(
// //     QPainter& painter,
// //     const WeaveCanvasPinViewData& pin,
// //     const QPointF& pinCenter,
// //     const QRect& textRect,
// //     const int pixelSize,
// //     const int alignment,
// //     const float zoom
// // ) {
// //     const float scaledPinRadius = std::max(2.0F, kNodePinRadius * zoom);
// //
// //     painter.save();
// //     painter.setPen(Qt::NoPen);
// //     painter.setBrush(pin.color);
// //     painter.drawEllipse(pinCenter, scaledPinRadius, scaledPinRadius);
// //     painter.restore();
// //
// //     if (textRect.width() > 2 && textRect.height() > 2) {
// //         DrawNodeText(painter, textRect, pin.name, pixelSize, alignment, false);
// //     }
// // }
// //
// // void DrawNodePins(
// //     QPainter& painter,
// //     const WeaveCanvasNodeViewData& node,
// //     const QRectF& nodeRect,
// //     const QRectF& headerRect,
// //     const float zoom
// // ) {
// //     const float scaledPinRadius = std::max(2.0F, kNodePinRadius * zoom);
// //     const float scaledPinTextSpacing = std::max(2.0F, kNodePinTextSpacing * zoom);
// //     const float scaledPinsHorizontalPadding = std::max(2.0F, kNodePinsHorizontalPadding * zoom);
// //     const float scaledPinsVerticalPadding = std::max(2.0F, kNodePinsVerticalPadding * zoom);
// //     const float scaledPinRowHeight = std::max(1.0F, kNodePinRowHeight * zoom);
// //     const int pinTextPixelSize = std::max(1, static_cast<int>(std::lround(headerRect.height() * 0.5F)));
// //     const float firstPinCenterY = headerRect.bottom() + scaledPinsVerticalPadding + scaledPinRowHeight * 0.5F;
// //
// //     for (int inputIndex = 0; inputIndex < node.inputPins.size(); ++inputIndex) {
// //         const WeaveCanvasPinViewData& inputPin = node.inputPins.at(inputIndex);
// //         const QPointF pinCenter(
// //             nodeRect.x() + scaledPinsHorizontalPadding + scaledPinRadius,
// //             firstPinCenterY + static_cast<float>(inputIndex) * scaledPinRowHeight
// //         );
// //         const QRect textRect = QRectF(
// //             pinCenter.x() + scaledPinRadius + scaledPinTextSpacing,
// //             pinCenter.y() - scaledPinRowHeight * 0.5F,
// //             nodeRect.width() * 0.5F - scaledPinsHorizontalPadding * 2.0F,
// //             scaledPinRowHeight
// //         ).toAlignedRect();
// //         DrawPin(painter, inputPin, pinCenter, textRect, pinTextPixelSize, Qt::AlignVCenter | Qt::AlignLeft, zoom);
// //     }
// //
// //     for (int outputIndex = 0; outputIndex < node.outputPins.size(); ++outputIndex) {
// //         const WeaveCanvasPinViewData& outputPin = node.outputPins.at(outputIndex);
// //         const QPointF pinCenter(
// //             nodeRect.right() - scaledPinsHorizontalPadding - scaledPinRadius,
// //             firstPinCenterY + static_cast<float>(outputIndex) * scaledPinRowHeight
// //         );
// //         const QRect textRect = QRectF(
// //             nodeRect.x() + nodeRect.width() * 0.5F,
// //             pinCenter.y() - scaledPinRowHeight * 0.5F,
// //             nodeRect.width() * 0.5F - scaledPinsHorizontalPadding * 2.0F - scaledPinRadius - scaledPinTextSpacing,
// //             scaledPinRowHeight
// //         ).toAlignedRect();
// //         DrawPin(
// //             painter,
// //             outputPin,
// //             pinCenter,
// //             textRect,
// //             pinTextPixelSize,
// //             Qt::AlignVCenter | Qt::AlignRight,
// //             zoom
// //         );
// //     }
// // }
// //
// // void DrawNodeBody(
// //     QPainter& painter,
// //     const QRectF& borderRect,
// //     const QRectF& nodeRect,
// //     const QRectF& headerRect,
// //     const float borderThickness,
// //     const float cornerRadius
// // ) {
// //     QPainterPath borderPath;
// //     borderPath.addRoundedRect(borderRect, cornerRadius, cornerRadius);
// //
// //     QPainterPath bodyPath;
// //     bodyPath.addRoundedRect(
// //         nodeRect,
// //         std::max(1.0F, cornerRadius - borderThickness),
// //         std::max(1.0F, cornerRadius - borderThickness)
// //     );
// //
// //     painter.setPen(Qt::NoPen);
// //     painter.setBrush(kNodeBorderColor);
// //     painter.drawPath(borderPath);
// //
// //     painter.setBrush(kNodeBodyColor);
// //     painter.drawPath(bodyPath);
// //
// //     painter.setBrush(kNodeHeaderColor);
// //     painter.drawRoundedRect(headerRect, cornerRadius, cornerRadius);
// //     painter.drawRect(
// //         QRectF(
// //             headerRect.x(),
// //             headerRect.y() + headerRect.height() * 0.5F,
// //             headerRect.width(),
// //             headerRect.height() * 0.5F
// //         )
// //     );
// // }
// // }
// //
// // float WeaveNodePainter::CalculateNodeBodyHeight(const WeaveCanvasNodeViewData& node) {
// //     const int maxPinCount = std::max(node.inputPins.size(), node.outputPins.size());
// //     if (maxPinCount <= 0) {
// //         return kNodeBaseBodyHeight;
// //     }
// //
// //     const float pinAreaHeight =
// //         kNodePinAreaTopPadding +
// //         static_cast<float>(maxPinCount) * kNodePinRowHeight +
// //         kNodePinAreaBottomPadding;
// //
// //     return kNodeBaseBodyHeight + pinAreaHeight;
// // }
// //
// void WeaveNodePainter::DrawNode(
//     QPainter& painter,
//     const WeaveCanvasNodeViewData& node,
//     const WeaveNodePaintParams& params
// ) {
//     const QRectF& borderRect = params.borderRect;
//     const float zoomScale = params.zoom;
//     const float borderThickness = std::max(1.0F, zoomScale);
//     const float scaledCornerRadius = std::max(2.0F, kNodeCornerRadius * zoomScale);
//     const float scaledHeaderHeight = std::max(1.0F, kNodeHeaderHeight * zoomScale);
//     const float scaledTitlePaddingX = std::max(1.0F, kNodeTitlePaddingX * zoomScale);
//     const float scaledDescriptionPaddingX = std::max(1.0F, kNodeDescriptionPaddingX * zoomScale);
//     const float scaledDescriptionPaddingTop = std::max(1.0F, kNodeDescriptionPaddingTop * zoomScale);
//     const float scaledDescriptionPaddingBottom = std::max(1.0F, kNodeDescriptionPaddingBottom * zoomScale);
//     const float scaledPinsVerticalPadding = std::max(2.0F, kNodePinsVerticalPadding * zoomScale);
//     const float scaledPinRowHeight = std::max(1.0F, kNodePinRowHeight * zoomScale);
//     const int maxPinCount = std::max(node.inputPins.size(), node.outputPins.size());
//     const QRectF nodeRect(
//         borderRect.x() + borderThickness,
//         borderRect.y() + borderThickness,
//         borderRect.width() - borderThickness * 2.0F,
//         borderRect.height() - borderThickness * 2.0F
//     );
//     const QRectF headerRect(nodeRect.x(), nodeRect.y(), nodeRect.width(), scaledHeaderHeight);
//     const QRectF titleRectF(
//         headerRect.x() + scaledTitlePaddingX,
//         headerRect.y(),
//         headerRect.width() - scaledTitlePaddingX * 2.0F,
//         headerRect.height()
//     );
//     const float pinsBottomY =
//         headerRect.bottom() +
//         scaledPinsVerticalPadding +
//         static_cast<float>(maxPinCount) * scaledPinRowHeight;
//
//     const QRectF descriptionRectF(
//         nodeRect.x() + scaledDescriptionPaddingX,
//         pinsBottomY + scaledDescriptionPaddingTop,
//         nodeRect.width() - scaledDescriptionPaddingX * 2.0F,
//         std::max(0.0, nodeRect.bottom() - pinsBottomY - scaledDescriptionPaddingTop - scaledDescriptionPaddingBottom)
//     );
//     const QRect titleRect = titleRectF.toAlignedRect();
//     const QRect descriptionRect = descriptionRectF.toAlignedRect();
//
//     painter.save();
//     painter.setRenderHint(QPainter::Antialiasing, true);
//     painter.setRenderHint(QPainter::TextAntialiasing, true);
//     DrawNodeBody(painter, borderRect, nodeRect, headerRect, borderThickness, scaledCornerRadius);
//     DrawNodePins(painter, node, nodeRect, headerRect, zoomScale);
//
//     const int scaledTitlePixelSize = std::max(1, static_cast<int>(std::lround(scaledHeaderHeight * 0.6F)));
//     if (titleRect.width() > 2 && titleRect.height() > 2) {
//         QFont titleFont = painter.font();
//         titleFont.setPixelSize(scaledTitlePixelSize);
//         const QFontMetrics fontMetrics(titleFont);
//         const QString elidedTitle = fontMetrics.elidedText(node.title, Qt::ElideRight, titleRect.width());
//         DrawNodeText(painter, titleRect, elidedTitle, scaledTitlePixelSize, Qt::AlignVCenter | Qt::AlignLeft, false);
//     }
//
//     if (!node.description.isEmpty() && descriptionRect.width() > 2 && descriptionRect.height() > 2) {
//         DrawNodeText(
//             painter,
//             descriptionRect,
//             node.description,
//             std::max(1, static_cast<int>(std::lround(scaledHeaderHeight * 0.4F))),
//             Qt::AlignLeft | Qt::AlignBottom,
//             true
//         );
//     }
//     painter.restore();
// }
