#include "Docking/Demo/InspectorDockingWidget.hpp"

#include <QLabel>
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

} // namespace Mirage::EditorQt
