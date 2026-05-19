#pragma once

#include "Docking/DockingWidget.hpp"

namespace Mirage::EditorQt {

class SceneDockingWidget final : public DockingWidget {
public:
    explicit SceneDockingWidget(QString title, bool closable, QWidget* parent = nullptr);
};

} // namespace Mirage::EditorQt
