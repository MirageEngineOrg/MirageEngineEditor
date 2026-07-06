#include "Docking/Demo/InspectorDockingWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Mirage::EditorQt {

InspectorDockingWidget::InspectorDockingWidget(
    QString title,
    bool closable,
    QWidget* parent
)
    : DockingWidget(std::move(title), closable, parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* label = new QLabel("Inspector DockingWidget demo content", this);
    label->setObjectName("DockingDemoText");
    layout->addWidget(label);
    layout->addStretch(1);
}

QWidget* InspectorDockingWidget::CreateToolBarWidget(QWidget* parent) {
    auto* toolBar = new QWidget(parent);
    auto* layout = new QHBoxLayout(toolBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* addComponentButton = new QPushButton("Add Component", toolBar);
    auto* resetButton = new QPushButton("Reset", toolBar);

    layout->addWidget(addComponentButton);
    layout->addWidget(resetButton);

    return toolBar;
}

} // namespace Mirage::EditorQt
