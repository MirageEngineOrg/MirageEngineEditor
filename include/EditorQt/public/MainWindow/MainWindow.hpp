#pragma once

#include "Application/EditorApplication.hpp"

#include <QMainWindow>

namespace Mirage::EditorQt {

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    Mirage::EditorCore::EditorApplication editorApplication_;
};

} // namespace Mirage::EditorQt
