#pragma once
#include "Base/Vector2D.h"

#include <QRectF>
#include <QSize>

#include <atomic>
#include <cstdint>

class CanvasCamera;
class QPainter;

typedef int64_t RenderedItemID;

class RenderedItemIDGenerator final {
public:
    [[nodiscard]] static RenderedItemID Next() noexcept {
        return nextId_.fetch_add(1, std::memory_order_relaxed);
    }

private:
    inline static std::atomic<RenderedItemID> nextId_ {1};
};

class IRenderedItem {
public:
    virtual ~IRenderedItem() = default;
    [[nodiscard]] virtual RenderedItemID GetID() const = 0;
    [[nodiscard]] virtual Vector2D GetSize() const = 0;
    [[nodiscard]] virtual Vector2D GetPosition() const = 0;
    virtual void SetPosition(const Vector2D& position) = 0;
    virtual void SetRect(const QRectF& rect) = 0;
    virtual void Paint(QPainter& painter, QSize viewportSize, CanvasCamera* camera) const = 0;
    virtual void OnSelect() = 0;
    virtual void OnUnselect() = 0;
};
