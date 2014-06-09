#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QDateTime>
#include <QNetworkInterface>
// Static global strings used for output

static const char* REPLACE_TOKEN("***-***");

// Define some HTML code to assist color coding of output
static const char* BLUE_TEXT_HTML(

 "<p><span style=\"color:#000080;font-size:18px;\">"
 "<span style=\"background-color:#ffffff;\">"
 "***-***"                                    // <- replace token
 "</span></span></p><BR>");

static const char* RED_TEXT_HTML(

 "<p><span style=\"color:#ff0000;font-size:18px;\">"
 "<span style=\"background-color:#ffff00;\">"
 "***-***"                                    // <- replace token
 "</span></span></p><BR>");

static const char* GREEN_TEXT_HTML(

 "<p><span style=\"color:#006400;font-size:18px;\">"
 "<span style=\"background-color:#ffffff;\">"
 "***-***"                                    // <- replace token
 "</span></span></p><BR>");




MainWindow::MainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mRecordManager(nullptr),
    mConfigDialog(this),
    mServerDialog(this),
    mConnectionManager(this)
{
    // Setup the Main UI and Add remaing items not declared in the .ui file
    ui->setupUi(this);

    // create the system tray
    mTrayIcon = new QSystemTrayIcon(this);
    mTrayIcon->setIcon(QIcon(R"(:/images/droneicon.png)"));
    mTrayIcon->setToolTip("Test");
    mTrayIcon->setVisible(true);
    if(!QSystemTrayIcon::isSystemTrayAvailable())
        writeMessage(RED_TEXT_HTML,"System Tray Icons are not available on this system.");
    else
        writeMessage(GREEN_TEXT_HTML,"System Tray Icons are available on this system.");

    // Create the Menu system (on rightclick) of system tray
    QMenu *changer_menu = new QMenu;
    mShowMain_action = new QAction(tr("Show MainWindow"),this);
    mShowMain_action->setIconVisibleInMenu(false);
    connect(mShowMain_action, SIGNAL(triggered()), this, SLOT(show()));
    changer_menu->addAction(mShowMain_action);
    changer_menu->addSeparator();

    mShowConfig_action = new QAction(tr("Show Video Configure"),this);
    mShowConfig_action->setIconVisibleInMenu(false);
    connect(mShowConfig_action, SIGNAL(triggered()), &mConfigDialog, SLOT(show()));
    connect(mShowConfig_action, SIGNAL(triggered()), this, SLOT(show()));
    changer_menu->addAction(mShowConfig_action);
    changer_menu->addSeparator();

    mToggleRecording_action = new QAction(tr("Start Video Capture"),this);
    mToggleRecording_action->setIconVisibleInMenu(false);
    connect(mToggleRecording_action, SIGNAL(triggered()), this, SLOT(toggleRecording()));
    changer_menu->addAction(mToggleRecording_action);
    changer_menu->addSeparator();

    mQuit_action = new QAction(tr("&Quit"), this);
    mQuit_action->setIconVisibleInMenu(true);
    connect(mQuit_action, SIGNAL(triggered()), app, SLOT(quit()));
    changer_menu->addAction(mQuit_action);

    // add the menus and show the system tray icon
    mTrayIcon->setContextMenu(changer_menu);
    mTrayIcon->show();

    // In 800 Milliseconds - check the configuration
    QTimer::singleShot(800,this, SLOT(checkConfiguration()));

    // connect signals and slots associate with main window
    connect(ui->actionStart_Recording,SIGNAL(triggered()),
            this,SLOT(startRecording()));
    connect(ui->actionStop_Recording,SIGNAL(triggered()),
            this,SLOT(stopRecording()));
    connect(ui->actionOpen,SIGNAL(triggered()),
            &mConfigDialog,SLOT(openDialog()));
    connect(ui->actionExit,SIGNAL(triggered()),
            app,SLOT(quit()));
    connect(ui->actionOpen_Server_Monitor,SIGNAL(triggered()),
            &mServerDialog,SLOT(show()));

    // connect up the Configure Dialog
    mConfigUi = mConfigDialog.ui;
    connect(mConfigUi->ButtonOk,SIGNAL(clicked()),
            this,SLOT(configureVideoRecordingManager()));
    connect(mConfigUi->ButtonCancel,SIGNAL(clicked()),
            &mConfigDialog,SLOT(hide()));


    // connect up the server dialog
    mServerUi = mServerDialog.ui; // get a convenience pointer
    connect(&mConnectionManager,SIGNAL(connectionChangedState(QString,QString)),
            &mServerDialog,SLOT(connectionStateChanged(QString,QString)));
    connect(&mConnectionManager,SIGNAL(connectionCreated(QString)),
            &mServerDialog,SLOT(connectionCreated(QString)));
    connect(&mConnectionManager,SIGNAL(connectionLost(QString)),
            &mServerDialog,SLOT(socketDisconnected(QString)));
    connect(&mConnectionManager,SIGNAL(connectionErrored(QString,QString)),
            &mServerDialog,SLOT(connectionErrorOccured(QString,QString)));
    connect(&mConnectionManager,SIGNAL(serverBeganListening(QHostAddress,int)),
            &mServerDialog,SLOT(serverListening(QHostAddress,int)));
    connect(mServerUi->actionClose,SIGNAL(triggered()),
            &mServerDialog,SLOT(hide()));

    // get the list of network IPs and begin listening on one
    QHostAddress local_ip;
    QList<QHostAddress> addresses(QNetworkInterface::allAddresses());
    foreach(QHostAddress address, addresses)
    {
        if(!address.isLoopback() && address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            local_ip = address;
            break;
        }
    }

    mConnectionManager.startServer(local_ip);
}


bool MainWindow::checkConfiguration()
{
    // check for invalid settings, flag setupcomple false if found    
    bool setup_complete(true);
    
    // Check process path
    QString process_path(mConfigUi->ProcessPath->text());
    QDir video_directory(process_path);
    video_directory.setCurrent(process_path);
    qDebug() << process_path;
    if(process_path.isEmpty() || process_path == "")
    {
        writeMessage(RED_TEXT_HTML, "Process Path is incorrectly configured.");
        setup_complete = false;
    }
    
    // check video directory
    QString video_directory_path(mConfigUi->VideoDirectory->text());
    video_directory.cd(video_directory_path);
    qDebug() << video_directory_path;
    if(video_directory_path.isEmpty() ||video_directory_path == "" || !video_directory.exists())
    {
        writeMessage(RED_TEXT_HTML, "Video Directory is incorrectly configured.");
        setup_complete = false;
    }

    // check for the camera
    QString device(mConfigUi->SelectDevice->currentText());
    if(device.isEmpty() ||device == "" )
    {
        writeMessage(RED_TEXT_HTML, "Could Not Find Camera Device!");
        setup_complete = false;
        qWarning() << "No Camera was found.";
        return false;
    }

    // check resolution
    QString resolution(mConfigUi->SelectResolution->currentText());
    if(resolution.isEmpty() ||resolution == "" )
    {
        writeMessage(RED_TEXT_HTML, "No Resolution Specified");
        setup_complete = false;
    }

    // check format
    QString format(mConfigUi->SelectInputFormat->currentText());
    if(format.isEmpty() ||format == "" )
    {
        writeMessage(RED_TEXT_HTML, "Invalid Input Format Specified.");
        setup_complete = false;
    }

    // check autonomous mode
    Qt::CheckState state = mConfigUi->ToggleAutonomousMode->checkState();
    mAutonomousMode =  state == Qt::Checked ? true:false;


    // check encoder
    QString encoder(mConfigUi->SelectVideoEncoder->currentText());
    if(encoder.isEmpty() ||encoder == "" )
    {
        writeMessage(RED_TEXT_HTML, "No Video Encoder Specified");
        setup_complete = false;
    }

    // notify user and return if setup completed successfully or failed
    if(setup_complete)
        mTrayIcon->showMessage("Passed Configuration","Configuration Passed please choose a file location!");
    return setup_complete;
}


void MainWindow::configureVideoRecordingManager()
{
    // each time a new configuration happens create a new record manager and hide the dialog
    delete mRecordManager;
    mRecordManager = new VideoRecordingManager(this,(mConfigUi->VideoDirectory->text() +"/"),
                                               mConfigUi->ProcessPath->text(),
                                               mConfigUi->SelectVideoEncoder->currentText(),
                                               mConfigUi->SelectInputFormat->currentText(),
                                               mConfigUi->SelectResolution->currentText(),
                                               mConfigUi->SelectDevice->currentText());
    mConfigDialog.hide();

}

void MainWindow::startRecording()
{

    writeMessage(BLUE_TEXT_HTML,"Requested to Start Recording, Fetching File Path.");

    // Check if Config has been properly completed
    if(!checkConfiguration())
    {
        writeMessage(RED_TEXT_HTML,"Check Configuration dialog!");
        mConfigDialog.openDialog();
        goto END;
    }
    else
    {
        writeMessage(GREEN_TEXT_HTML,"Configuration Check: Configuration Complete");
        QString filename;
        if(!mAutonomousMode)
            filename =                     // Get filename and file path
                    QFileDialog::getSaveFileName(
                            this,
                            "Select Video Name and Path",
                            mRecordManager->mCurrentDirectory,"Videos (*.avi *.mpg *.mpeg)");
        else
        {
            filename = QDateTime::currentDateTime().toString().replace(" ","-").replace(":","-");
            filename.append(".mpg");
        }
        qDebug() << "File name fetched: " << filename;
        // Check File Path
        if(filename.isEmpty())
        {
            writeMessage(RED_TEXT_HTML,"Failed to get a valid file path, try again.");
            goto END;
        }
        else
        {
             writeMessage(GREEN_TEXT_HTML,("Got File Path: " + filename));
        }

        // Check if Recorder is already created
        if(mRecordManager == nullptr)
        {
            mRecordManager = new VideoRecordingManager(this,mConfigUi->VideoDirectory->text() + "/",
                                                       mConfigUi->ProcessPath->text(),
                                                       mConfigUi->SelectVideoEncoder->currentText(),
                                                       mConfigUi->SelectInputFormat->currentText(),
                                                       mConfigUi->SelectResolution->currentText(),
                                                       mConfigUi->SelectDevice->currentText());
            connect(mRecordManager,SIGNAL(mediaInfoAvailable(QString,QString)),this,SLOT(writeMediaInfo(QString,QString)));
        }
        int last_slash(filename.lastIndexOf(QDir::separator()));

        QString directory;
        if(!mAutonomousMode)
            directory  = filename.left(last_slash+1);
        filename.replace(directory ,"");
        mTrayIcon->showMessage("Started Recording","Video has started recording\n" + filename + "  " + directory);
        qDebug() << filename << "\n" << directory;
        mRecording = true;
        qDebug() << "File name entering start recording" << filename;
        mRecordManager->startRecording(filename);


    }

    END:
    return;
}


void MainWindow::toggleRecording()
{
    // toggle recording on or off
    if(mRecording)
    {
        stopRecording();
        mToggleRecording_action->setText("Start Video Capture");
    }
    else
    {
        startRecording();
        mToggleRecording_action->setText("Stop Video Capture");
    }

}



void MainWindow::stopRecording()
{
    // if there is a recording manager, stop the recording and notify user recording is done
    if(mRecordManager)
    {
        mRecordManager->stopRecording();
        mTrayIcon->showMessage("Stopped Recording","Video has stopped recording\n");
        mRecording = false;
    }

}


void MainWindow::writeMessage(const QString & HTML_ENCODING, QString message_out)
{
    QString message(HTML_ENCODING);
    message.replace(REPLACE_TOKEN, message_out);
    ui->textEdit->insertHtml(message);
}


void MainWindow::writeMediaInfo(QString video_path, QString mediadata,QPixmap thumbnail)
{
    writeMessage(BLUE_TEXT_HTML, QString("Finshed Recording Video: " + video_path));
    writeMessage(GREEN_TEXT_HTML, QString(mediadata).toHtmlEscaped());
    writeMessage(GREEN_TEXT_HTML, "Thumbnail Created:");
    ui->thumbnailLabel->setPixmap(thumbnail);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mShowMain_action;
    delete mShowConfig_action;
    delete mQuit_action;
    delete mTrayIcon;
}
