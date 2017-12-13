#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
