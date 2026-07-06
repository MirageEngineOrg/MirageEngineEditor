#pragma once

#include "Explorer/ExplorerItemWidget.hpp"

namespace Mirage::EditorQt {

class ExplorerFileWidget final : public ExplorerItemWidget {
public:
    explicit ExplorerFileWidget(QString name, QWidget* parent = nullptr);
};

} // namespace Mirage::EditorQt
