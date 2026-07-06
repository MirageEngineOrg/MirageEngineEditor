#pragma once
#include "Commands/EditorCommandManager.hpp"
#include "Docking/DockingWidget.hpp"
#include "Editor/WeaveCanvas/WeaveCanvas.h"
#include "Editor/SearchNodesModalWidget/SearchNodesModalWidget.hpp"

#include "WaveGraph/WeaveNodeRegistryTypes.hpp"
#include "WaveGraph/WeaveGraphTypes.hpp"

#include <unordered_map>

class QWidget;
class QObject;
class QEvent;
class QMouseEvent;

using namespace Mirage::EditorCore::WaveGraph;

struct WeaveEditorNodeModel {
    QString registryPath;
    WeaveNodeId nodeId{0};
    std::unordered_map<int, WeaveNodeInputDescriptor> inputs;
    std::unordered_map<int, WeaveNodeOutputDescriptor> outputs;
};

class WeaveEditorWidget : public Mirage::EditorQt::DockingWidget {
public:
    explicit WeaveEditorWidget(bool closable, QWidget* parent = nullptr);
    [[nodiscard]] QWidget* CreateToolBarWidget(QWidget* parent) override;
    void ShowSearchNodesModal();
    void HideSearchNodesModal();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    void InitUI();
    void InitGraph();
    [[nodiscard]] QWidget* CreateCanvasLayer();
    [[nodiscard]] QWidget* CreateSearchNodesModalLayer();
    void CreateShowSearchNodesModalAction();
    void CreateUndoRedoActions();
    void HandleCanvasNodeRenderDataChanged(WeaveNodeId nodeId);
    void HandleMovementSessionFinished();
    void HandleSearchNodeSelected(const WeaveNodeInfo &nodeInfo);
    void HandleUndoRequested();
    void HandleRedoRequested();

    void AddNode(const WeaveNodeInfo &nodeInfo);

    void ValidateConnection(WeaveCanvasConnection connection, bool& result);

    WeaveCanvas* canvas_ {nullptr};
    Mirage::EditorQt::EditorCommandManager commandManager_ {};
    QWidget* searchNodesModalWrapper_ {nullptr};
    Mirage::EditorQt::SearchNodesModalWidget* searchNodesModalWidget_ {nullptr};
    Mirage::EditorCore::WaveGraph::WeaveGraphId graphId_ {0};
    std::unordered_map<WeaveNodeId, WeaveEditorNodeModel> nodeModels_ {};
};
