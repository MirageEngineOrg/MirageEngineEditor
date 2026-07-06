#pragma once
#include <QObject>
#include <QPoint>

class QMouseEvent;
class QWheelEvent;

class CanvasBaseInputController : public QObject {
    Q_OBJECT
public:
    explicit CanvasBaseInputController(QObject* parent = nullptr);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

signals:
    void PanningStarted();
    void PanningFinished();
    void PanDeltaRequested(float deltaX, float deltaY);

    void ZoomRequested(float delta);

    void OnLeftMousePressed(float mousePosX, float mousePosY, bool controlPressed);
    void OnDrag(float mouseX, float mouseY);
    void OnLeftMouseReleased(float mouseX, float mouseY);
private:
    bool isPanning_ {false};

    QPoint lastMousePosition_ {};
    bool wasLeftMouseClicked {false};
    QPoint startSelectingPos_ {};
};
