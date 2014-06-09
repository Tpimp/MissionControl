#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QAction>
#include "Tier2_Business/videorecordingmanager.h"
#include "configdialog.h"
#include "ui_configdialog.h"
#include "serverdialog.h"
#include "ui_serverdialog.h"
#include "Tier2_Business/clientconnectionmanager.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:

    explicit MainWindow(QApplication *app, QWidget *parent = 0);
    void writeMessage(const QString & HTML_ENCODING, QString message_out);
    ~MainWindow();

public slots:

    void toggleRecording();
    void startRecording();
    void stopRecording();

protected slots:

    bool checkConfiguration();
    void configureVideoRecordingManager();
    void writeMediaInfo(QString video_path, QString mediadata, QPixmap thumbnail);


private:
    Ui::MainWindow*          ui;
    QApplication*           mApp;
    VideoRecordingManager*  mRecordManager;
    ClientConnectionManager mConnectionManager;
    ConfigDialog            mConfigDialog;
    Ui::ConfigDialog*       mConfigUi;
    ServerDialog            mServerDialog;
    Ui::ServerDialog*       mServerUi;
    QSystemTrayIcon*        mTrayIcon;
    QAction*                mShowMain_action;
    QAction*                mShowConfig_action;
    QAction*                mToggleRecording_action;
    QAction*                mQuit_action;
    bool                    mRecording = false;
    bool                    mAutonomousMode = false;

};

#endif // MAINWINDOW_H
