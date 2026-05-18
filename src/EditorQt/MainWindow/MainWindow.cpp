#include "MainWindow/MainWindow.hpp"

#include "MainWindow/MainWindowInternal.hpp"

#include "Application/EditorApplication.hpp"

namespace Mirage::EditorQt {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , editorApplication_(Mirage::EditorCore::EditorApplicationConfig{"Mirage Project"}) {
    setWindowTitle(kMainWindowTitle);
    resize(kDefaultWindowWidth, kDefaultWindowHeight);

    const auto& config = editorApplication_.GetConfig();
    Q_UNUSED(config);
}

} // namespace Mirage::EditorQt
