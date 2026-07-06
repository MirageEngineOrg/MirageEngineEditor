#include "WeaveCanvasBase/CanvasBaseInputController.h"
#include <QWheelEvent>

CanvasBaseInputController::CanvasBaseInputController(QObject* parent)
    : QObject(parent) {
}

void CanvasBaseInputController::mousePressEvent(QMouseEvent* event) {
    switch (event->button()) {
        case Qt::LeftButton: {
            if (isPanning_) return;
            wasLeftMouseClicked = true;
            lastMousePosition_ = event->pos();
            emit OnLeftMousePressed(
                static_cast<float>(event->pos().x()),
                static_cast<float>(event->pos().y()),
                event->modifiers().testFlag(Qt::ControlModifier)
            );
            break;
        }
        case Qt::MiddleButton: {
            isPanning_ = true;
            lastMousePosition_ = event->pos();
            emit PanningStarted();
            event->accept();
            break;
        }

    default:
        return;
    }
}

void CanvasBaseInputController::mouseMoveEvent(QMouseEvent* event) {
    if (isPanning_) {
        const QPoint mouseDelta = event->pos() - lastMousePosition_;
        lastMousePosition_ = event->pos();
        emit PanDeltaRequested(static_cast<float>(mouseDelta.x()), static_cast<float>(mouseDelta.y()));
        event->accept();
    } else if (wasLeftMouseClicked) {
        emit OnDrag(event->pos().x(), event->pos().y());
        event->accept();
    }


}

void CanvasBaseInputController::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && wasLeftMouseClicked) {
        wasLeftMouseClicked = false;

        emit OnLeftMouseReleased(
            event->pos().x(),
            event->pos().y()
            );

        event->accept();
    }

    if (event->button() == Qt::MiddleButton && isPanning_) {
        isPanning_ = false;
        emit PanningFinished();
        event->accept();
    }

}

void CanvasBaseInputController::wheelEvent(QWheelEvent* event) {
    emit ZoomRequested(static_cast<float>(event->angleDelta().y()));
    event->accept();
}

