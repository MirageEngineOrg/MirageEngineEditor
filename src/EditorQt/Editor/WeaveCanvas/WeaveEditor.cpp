#include "Editor/WeaveCanvas/WeaveEditor.h"

#include "Application/EditorCore.hpp"
#include "Commands/MoveRenderedItemsCommand.hpp"
#include "Editor/RenderedWeaveNodeItem.h"
#include "Editor/SearchNodesModalWidget/SearchNodesModalWidget.hpp"
#include "Editor/WeaveCanvas/WeaveColorPicker.h"

#include <QAction>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QStackedLayout>
#include <QVBoxLayout>

const char *_widget_title = "WeaveEditor";

WeaveEditorWidget::WeaveEditorWidget(bool closable, QWidget *parent)
    : DockingWidget(_widget_title, closable, parent) {
    setFocusPolicy(Qt::StrongFocus);
    InitGraph();
    InitUI();
}

QWidget *WeaveEditorWidget::CreateToolBarWidget(QWidget *parent) {
    return DockingWidget::CreateToolBarWidget(parent);
}

bool WeaveEditorWidget::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched);

    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_A && keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
            ShowSearchNodesModal();
            keyEvent->accept();
            return true;
        }
    }

    return DockingWidget::eventFilter(watched, event);
}

void WeaveEditorWidget::ShowSearchNodesModal() {
    if (searchNodesModalWrapper_ == nullptr) {
        return;
    }

    searchNodesModalWrapper_->raise();
    searchNodesModalWrapper_->show();
    if (searchNodesModalWidget_ != nullptr) {
        searchNodesModalWidget_->raise();
        searchNodesModalWidget_->show();
        searchNodesModalWidget_->setFocus(Qt::ShortcutFocusReason);
    }
}

void WeaveEditorWidget::HideSearchNodesModal() {
    if (searchNodesModalWidget_ != nullptr) {
        searchNodesModalWidget_->hide();
    }

    if (searchNodesModalWrapper_ != nullptr) {
        searchNodesModalWrapper_->hide();
    }
}
void WeaveEditorWidget::mousePressEvent(QMouseEvent *event) {
    DockingWidget::mousePressEvent(event);
    if (searchNodesModalWidget_->isVisible()) {
        HideSearchNodesModal();
    }
}

void WeaveEditorWidget::InitGraph() {
    graphId_ = Mirage::EditorCore::EditorCore::GetInstance().GetWeaveGraphManager()->CreateGraph();
    qDebug() << "WeaveEditorWidget created graph with id =" << graphId_;
}

void WeaveEditorWidget::InitUI() {
    auto *stack = new QStackedLayout(this);
    stack->setStackingMode(QStackedLayout::StackAll);

    stack->addWidget(CreateCanvasLayer());

    stack->addWidget(CreateSearchNodesModalLayer());
    CreateShowSearchNodesModalAction();
    CreateUndoRedoActions();

    setLayout(stack);

    nodeModels_.clear();
}

QWidget *WeaveEditorWidget::CreateCanvasLayer() {
    auto *canvasWidgetWrapper = new QWidget(this);
    auto *canvasWidgetWrapperLayout = new QVBoxLayout(canvasWidgetWrapper);
    canvasWidgetWrapperLayout->setContentsMargins(0, 0, 0, 0);
    canvasWidgetWrapperLayout->setSpacing(0);

    canvas_ = new WeaveCanvas(this);
    canvas_->setFocusPolicy(Qt::StrongFocus);
    canvas_->installEventFilter(this);
    connect(canvas_, &IBaseWeaveCanvas::MovementSessionFinished, this,
            &WeaveEditorWidget::HandleMovementSessionFinished);
    connect(canvas_, &WeaveCanvas::ValidateConnection, this, &WeaveEditorWidget::ValidateConnection);
    //connect(canvas_, &WeaveCanvas::NodeRenderDataChanged, this,
    //        &WeaveEditorWidget::HandleCanvasNodeRenderDataChanged);
    //connect(canvas_, &WeaveCanvas::MovementSessionFinished, this,
    //        &WeaveEditorWidget::HandleMovementSessionFinished);
    //connect(canvas_, &WeaveCanvas::UndoRequested, this, &WeaveEditorWidget::HandleUndoRequested);
    //connect(canvas_, &WeaveCanvas::RedoRequested, this, &WeaveEditorWidget::HandleRedoRequested);
    canvasWidgetWrapperLayout->addWidget(canvas_);

    return canvasWidgetWrapper;
}

QWidget *WeaveEditorWidget::CreateSearchNodesModalLayer() {
    const auto nodeRegistry =
        Mirage::EditorCore::EditorCore::GetInstance().GetNodeRegistry();

    searchNodesModalWrapper_ = new QWidget(this);
    auto *searchModalWrapperLayout = new QVBoxLayout(searchNodesModalWrapper_);
    searchModalWrapperLayout->setContentsMargins(0, 0, 0, 0);
    searchModalWrapperLayout->setSpacing(0);
    searchModalWrapperLayout->setAlignment(Qt::AlignCenter);

    if (nodeRegistry != nullptr) {
        searchNodesModalWidget_ = new Mirage::EditorQt::SearchNodesModalWidget(
            nodeRegistry->GetRootGroup(), searchNodesModalWrapper_);
        searchModalWrapperLayout->addWidget(searchNodesModalWidget_);
        connect(searchNodesModalWidget_, &Mirage::EditorQt::SearchNodesModalWidget::OnShouldClose,
                this, &WeaveEditorWidget::HideSearchNodesModal);
        connect(searchNodesModalWidget_, &Mirage::EditorQt::SearchNodesModalWidget::OnNodeSelected,
                this, &WeaveEditorWidget::HandleSearchNodeSelected);
    }

    return searchNodesModalWrapper_;
}

void WeaveEditorWidget::CreateShowSearchNodesModalAction() {
    auto *showSearchNodesModalAction = new QAction(this);
    showSearchNodesModalAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_A));
    showSearchNodesModalAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(showSearchNodesModalAction, &QAction::triggered, this,
            &WeaveEditorWidget::ShowSearchNodesModal);
    addAction(showSearchNodesModalAction);
}

void WeaveEditorWidget::CreateUndoRedoActions() {
    auto* undoAction = new QAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(undoAction, &QAction::triggered, this, &WeaveEditorWidget::HandleUndoRequested);
    addAction(undoAction);

    auto* redoAction = new QAction(this);
    redoAction->setShortcuts({QKeySequence::Redo, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z)});
    redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(redoAction, &QAction::triggered, this, &WeaveEditorWidget::HandleRedoRequested);
    addAction(redoAction);
}

void WeaveEditorWidget::HandleCanvasNodeRenderDataChanged(
    const Mirage::EditorCore::WaveGraph::WeaveNodeId nodeId) {
    const auto it = nodeModels_.find(nodeId);
    if (it == nodeModels_.end()) {
        return;
    }
    qDebug() << "WeaveEditorWidget received render-data change for node id =" << nodeId
             << "registryPath =" << it->second.registryPath;
}

void WeaveEditorWidget::HandleMovementSessionFinished() {
    if (canvas_ == nullptr) {
        return;
    }

    std::optional<RenderedItemsMovementSession> session = canvas_->TakeCompletedMovementSession();
    if (!session.has_value() || session->IsEmpty()) {
        return;
    }

    commandManager_.PushExecutedCommand(
        std::make_unique<Mirage::EditorQt::MoveRenderedItemsCommand>(
            std::move(session.value()),
            [this](const RenderedItemID renderedItemID, const QRectF& itemRect) {
                if (canvas_ != nullptr) {
                    canvas_->ApplyRenderedItemRect(renderedItemID, itemRect);
                }
            }
        )
    );
}

void WeaveEditorWidget::HandleUndoRequested() {
    commandManager_.Undo();
}

void WeaveEditorWidget::HandleRedoRequested() {
    commandManager_.Redo();
}

void WeaveEditorWidget::HandleSearchNodeSelected(const WeaveNodeInfo &nodeInfo) {
    HideSearchNodesModal();
    AddNode(nodeInfo);
}

void WeaveEditorWidget::AddNode(const WeaveNodeInfo &nodeInfo) {
    if (canvas_ == nullptr) {
        return;
    }

    WeaveCreatedNodeInfo created_node_info = nodeInfo.constructor(graphId_);

    this->nodeModels_[created_node_info.nodeId] = {
        .registryPath = "",
        .nodeId = created_node_info.nodeId,
        .inputs = {},
        .outputs = {}
    };

    QVector<WeaveCanvasPinViewData> pins;

    for (auto& input_pin : created_node_info.inputs) {
        const int pinId = pins.size();
        this->nodeModels_[created_node_info.nodeId].inputs[pinId] = std::move(input_pin);
        pins.emplace_back(
            pinId,
            QString::fromStdString(input_pin.name),
            WeaveColorPicker::GetPinColor(input_pin.GetKind()),
            WeavePinDirection::Input
        );
    }

    for (auto& output_pin : created_node_info.outputs) {
        const int pinId = pins.size();
        this->nodeModels_[created_node_info.nodeId].outputs[pinId] = output_pin;
        pins.emplace_back(
            pinId,
            QString::fromStdString(output_pin.name),
            WeaveColorPicker::GetPinColor(output_pin.getKind()),
            WeavePinDirection::Output
        );
    }

    canvas_->AddNode(
        created_node_info.nodeId,
        std::make_shared<RenderedWeaveNodeItem>(
            QString::fromStdString(nodeInfo.name),
            QString::fromStdString(nodeInfo.description),
            pins,
            Vector2D {.x = 0.0F, .y = 0.0F}
        )
    );
}
void WeaveEditorWidget::ValidateConnection(WeaveCanvasConnection connection, bool &result) {
    WeaveEditorNodeModel& fromNode = nodeModels_[connection.sourceNodeId];
    WeaveEditorNodeModel& toNode = nodeModels_[connection.targetNodeId];

    auto& validationFunction = toNode.inputs[connection.targetPinId].connection_handler;
    auto test = Mirage::EditorCore::EditorCore::GetInstance().GetWeaveGraphManager()->GetGraph(graphId_);
    if (validationFunction) {
        result = validationFunction->ValidateConnection(test->GetNode(fromNode.nodeId));
    } else result = false;
}
