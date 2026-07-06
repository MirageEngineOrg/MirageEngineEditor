#pragma once
#include "Base/Vector2D.h"

#include <algorithm>

class CanvasCamera {
public:
    CanvasCamera() = default;

    void Move(float deltaX, float deltaY);
    void Zoom(float delta);

    float GetZoom() const { return this->zoom_; }
    Vector2D GetPan() const { return this->pan_; }
protected:
    float kCanvasMoveSensitivity = 1.0F;
    float kCanvasMinZoom = 0.1F;
    float kCanvasMaxZoom = 5.0F;
    float kCanvasZoomSensitivity = 0.001F;
    float kCanvasMinZoomDeltaScale = 0.2F;
private:
    float zoom_ {1.0F};
    Vector2D pan_ {};
};

inline void CanvasCamera::Move(float deltaX, float deltaY) {
    const float safeZoom = std::max(this->zoom_, kCanvasMinZoom);
    this->pan_.x -= (deltaX * kCanvasMoveSensitivity) / safeZoom;
    this->pan_.y -= (deltaY * kCanvasMoveSensitivity) / safeZoom;
}

inline void CanvasCamera::Zoom(const float delta) {
    float zoomRange = kCanvasMaxZoom - kCanvasMinZoom;
    const float normalizedZoom = (this->zoom_ - kCanvasMinZoom) / zoomRange;
    const float zoomDeltaScale = std::clamp(normalizedZoom, kCanvasMinZoomDeltaScale, 1.0F);

    this->zoom_ += delta * kCanvasZoomSensitivity * zoomDeltaScale;
    this->zoom_ = std::clamp(this->zoom_, kCanvasMinZoom, kCanvasMaxZoom);
}
