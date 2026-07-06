#include "Docking/Demo/SceneDockingWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Mirage::EditorQt {

SceneDockingWidget::SceneDockingWidget(QString title, bool closable, QWidget* parent)
    : DockingWidget(std::move(title), closable, parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* label = new QLabel("Scene DockingWidget demo content", this);
    label->setObjectName("DockingDemoText");
    layout->addWidget(label);
    layout->addStretch(1);
}

QWidget* SceneDockingWidget::CreateToolBarWidget(QWidget* parent) {
    auto* toolBar = new QWidget(parent);
    auto* layout = new QHBoxLayout(toolBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* selectButton = new QPushButton("Select", toolBar);
    auto* moveButton = new QPushButton("Move", toolBar);
    auto* rotateButton = new QPushButton("Rotate", toolBar);

    layout->addWidget(selectButton);
    layout->addWidget(moveButton);
    layout->addWidget(rotateButton);

    return toolBar;
}

} // namespace Mirage::EditorQt
