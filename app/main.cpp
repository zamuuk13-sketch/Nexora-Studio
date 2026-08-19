#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Nexora");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Nexora");
    MainWindow window;
    window.show();
    return app.exec();
}
