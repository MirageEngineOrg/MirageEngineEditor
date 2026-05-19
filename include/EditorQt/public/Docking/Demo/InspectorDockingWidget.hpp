#pragma once

#include "Docking/DockingWidget.hpp"

namespace Mirage::EditorQt {

class InspectorDockingWidget final : public DockingWidget {
public:
    explicit InspectorDockingWidget(QString title, bool closable, QWidget* parent = nullptr);
};

} // namespace Mirage::EditorQt
