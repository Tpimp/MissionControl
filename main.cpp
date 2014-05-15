#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(&a);
    a.setApplicationDisplayName("Mission Control");
    a.setQuitOnLastWindowClosed(false);
    w.show();
    return a.exec();

}
