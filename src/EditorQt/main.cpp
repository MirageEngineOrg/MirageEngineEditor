#include "MainWindow/MainWindow.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);

    Mirage::EditorQt::MainWindow window;
    window.show();

    return QApplication::exec();
}
