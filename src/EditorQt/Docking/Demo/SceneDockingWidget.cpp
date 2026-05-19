#include "Docking/Demo/SceneDockingWidget.hpp"

#include <QLabel>
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

} // namespace Mirage::EditorQt
