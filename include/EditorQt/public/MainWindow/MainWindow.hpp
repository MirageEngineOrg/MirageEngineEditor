#pragma once

#include "Application/EditorApplication.hpp"
#include "EditorTitleBar/EditorTitleBar.hpp"

#include <QMainWindow>

namespace Mirage::EditorQt {

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif

private:
    Mirage::EditorCore::EditorApplication editorApplication_;
    EditorTitleBar* editorTitleBar_ {nullptr};
};

} // namespace Mirage::EditorQt
