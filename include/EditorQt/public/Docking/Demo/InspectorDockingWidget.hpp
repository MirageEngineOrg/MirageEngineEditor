#pragma once

#include "Docking/DockingWidget.hpp"

namespace Mirage::EditorQt {

class InspectorDockingWidget final : public DockingWidget {
public:
    explicit InspectorDockingWidget(QString title, bool closable, QWidget* parent = nullptr);

    [[nodiscard]] QWidget* CreateToolBarWidget(QWidget* parent) override;
};

} // namespace Mirage::EditorQt
