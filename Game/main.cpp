#include "mainwindow.h"
#include <QApplication>
#include <memory>
#include <QTimer>
#include "gameengine.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setApplicationDisplayName("Role Game");

    MainWindow w;
    w.show();

    return a.exec();
}
