#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    // Create the main application and main window
    QApplication application(argc, argv);
    MainWindow window(&application);

    // Cofnigure for Mission Control and Daemon mode
    application.setApplicationDisplayName("Mission Control");
    application.setQuitOnLastWindowClosed(false);

    // show the application and begin event loop
    window.show();
    return application.exec();

}
