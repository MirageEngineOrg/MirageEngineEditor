#pragma once
#include "Editor/RenderedWeaveNodeItem.h"
#include "WaveGraph/WeaveGraphTypes.hpp"
#include "WeaveCanvasBase/WeaveCanvasBase.h"

#include <atomic>

using namespace Mirage::EditorCore::WaveGraph;

struct ConnectionID {
    int64_t value {0};

    [[nodiscard]] bool IsValid() const noexcept {
        return value != 0;
    }

    [[nodiscard]] bool operator==(const ConnectionID&) const noexcept = default;
};

class ConnectionIDGenerator final {
public:
    [[nodiscard]] static ConnectionID Next() noexcept {
        return ConnectionID {.value = nextId_.fetch_add(1, std::memory_order_relaxed)};
    }

private:
    inline static std::atomic<int64_t> nextId_ {1};
};

struct WeaveCanvasConnection {
    ConnectionID connectionId {ConnectionIDGenerator::Next()};
    WeaveNodeId sourceNodeId{0};
    int sourcePinId {-1};
    WeaveNodeId targetNodeId{0};
    int targetPinId {-1};

    [[nodiscard]] bool IsValid() const noexcept {
        return sourceNodeId != 0 && targetNodeId != 0 && sourcePinId != -1 && targetPinId != -1;
    }
};

inline size_t qHash(const ConnectionID connectionId, size_t seed = 0) noexcept {
    return qHash(static_cast<qint64>(connectionId.value), seed);
}

inline size_t qHash(const WeaveCanvasConnection& connection, size_t seed = 0) noexcept {
    seed = qHash(connection.connectionId, seed);
    seed ^= qHash(static_cast<quint64>(connection.sourceNodeId), seed + 0x9e3779b9U + (seed << 6U) + (seed >> 2U));
    seed ^= qHash(connection.sourcePinId, seed + 0x9e3779b9U + (seed << 6U) + (seed >> 2U));
    seed ^= qHash(static_cast<quint64>(connection.targetNodeId), seed + 0x9e3779b9U + (seed << 6U) + (seed >> 2U));
    seed ^= qHash(connection.targetPinId, seed + 0x9e3779b9U + (seed << 6U) + (seed >> 2U));
    return seed;
}

class WeaveCanvas : public IBaseWeaveCanvas {
    Q_OBJECT
public:
    WeaveCanvas(QWidget* parent = nullptr);
    void AddNode(WeaveNodeId nodeId, std::shared_ptr<RenderedWeaveNodeItem> node);
    void AddConnection(const WeaveCanvasConnection& connection);
protected:
    void OnItemsDragStarted() override;
    void HandleSelectionChanged() override;
    CanvasInteractionMode onPressedItem(OnPressItemContext on_press_item_context,
                                        IRenderedItem *item) override;


    void PaintCanvas(QPainter &painter) override;
    void OnDrag(float mouseX, float mouseY) override;
    void OnLeftMouseReleased(float mouseX, float mouseY) override;

    QHash<RenderedItemID, WeaveNodeId> renderedWaveNodes_;
    QHash<WeaveNodeId, RenderedItemID> renderedItemIdsByNodeId_;
    QHash<ConnectionID, WeaveCanvasConnection> connections_;
    RenderedWeaveNodeItem* draggingPinSourceNode  = nullptr;
    int draggingPinIndex_ = -1;

    QPointF draggingEnd = QPointF();
private:
    void DrawConnection(const Vector2D& from_global, const Vector2D& to_global, QPainter& painter);
    [[nodiscard]] RenderedWeaveNodeItem* FindNodeById(WeaveNodeId nodeId) const;

    signals:
    void ValidateConnection(WeaveCanvasConnection, bool&);
    void OnEstablishConnection(WeaveCanvasConnection);
};
