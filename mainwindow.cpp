#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QDateTime>
// Static global strings used for output

static const char* REPLACE_TOKEN("***-***");

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
    ui->setupUi(this);
    mTrayIcon = new QSystemTrayIcon(this);
    mTrayIcon->setIcon(QIcon(R"(:/images/droneicon.png)"));
    mTrayIcon->setToolTip("Test");
    mTrayIcon->setVisible(true);
    if(!QSystemTrayIcon::isSystemTrayAvailable())
        writeMessage(RED_TEXT_HTML,"System Tray Icons are not available on this system.");
    else
        writeMessage(GREEN_TEXT_HTML,"System Tray Icons are available on this system.");
    QMenu *changer_menu = new QMenu;


    mShowMain_action = new QAction(tr("Show MainWindow"),this);
    mShowMain_action->setIconVisibleInMenu(false);
    connect(mShowMain_action, SIGNAL(triggered()), this, SLOT(show()));
    changer_menu->addAction(mShowMain_action);
    changer_menu->addSeparator();

    mShowCfg_action = new QAction(tr("Show Video Configure"),this);
    mShowCfg_action->setIconVisibleInMenu(false);
    connect(mShowCfg_action, SIGNAL(triggered()), &mConfigDialog, SLOT(show()));
    connect(mShowCfg_action, SIGNAL(triggered()), this, SLOT(show()));
    changer_menu->addAction(mShowCfg_action);
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
    mTrayIcon->setContextMenu(changer_menu);
    mTrayIcon->show();
    QTimer::singleShot(800,this, SLOT(checkConfiguration()));
    connect(ui->actionStart_Recording, // file menu start recording
            SIGNAL(triggered()),       // triggered event
            this,                      // calls this classes
            SLOT(startRecording()));   // startRecording slot
    connect(ui->actionStop_Recording,  // file menu stop recording
            SIGNAL(triggered()),       // triggered event
            this,                      // calls this classes
            SLOT(stopRecording()));    // stopRecording slot    
    connect(ui->actionOpen,            // open config
            SIGNAL(triggered()),       // triggered event
            &mConfigDialog,            // Config dialogs
            SLOT(openDialog()));       // open
    connect(ui->actionExit,            // action to exit
            SIGNAL(triggered()),
            app,
            SLOT(quit()));
    connect(ui->actionOpen_Server_Monitor,
            SIGNAL(triggered()),
            &mServerDialog,
            SLOT(show()));

    // connect up the Configure Dialog
    mConfigUi = mConfigDialog.ui;      // get a convenience pointer
    connect(mConfigUi->ButtonOk,       // connect ok button
            SIGNAL(clicked()),
            this,
            SLOT(configureVideoRecordingManager()));
    connect(mConfigUi->ButtonCancel,  // connect cancel
            SIGNAL(clicked()),
            &mConfigDialog,
            SLOT(hide()));


    // connect up the server dialog
    mServerUi = mServerDialog.ui; // get a convenience pointer
    connect(&mConnectionManager,
            SIGNAL(connectionChangedState(QString,QString)),
            &mServerDialog,
            SLOT(connectionStateChanged(QString,QString)));

    connect(&mConnectionManager,
            SIGNAL(connectionCreated(QString)),
            &mServerDialog,
            SLOT(connectionCreated(QString)));

    connect(&mConnectionManager,
            SIGNAL(connectionLost(QString)),
            &mServerDialog,
            SLOT(socketDisconnected(QString)));

    connect(&mConnectionManager,
            SIGNAL(connectionErrored(QString,QString)),
            &mServerDialog,
            SLOT(connectionErrorOccured(QString,QString)));

    connect(&mConnectionManager,
            SIGNAL(serverBeganListening(QHostAddress,int)),
            &mServerDialog,
            SLOT(serverListening(QHostAddress,int)));

    connect(mServerUi->actionClose,
            SIGNAL(triggered()),
            &mServerDialog,
            SLOT(hide()));


    mConnectionManager.startServer(QHostAddress("192.168.3.115"), 8889);
}


bool MainWindow::checkConfiguration()
{
    // check for invalid settings, flag setupcomple false if found    
    bool SetupComplete(true);
    
    // Check process path
    QString processpath(mConfigUi->ProcessPath->text());
    QDir dir(processpath);
    dir.setCurrent(processpath);
    qDebug() << processpath;
    if(processpath.isEmpty() || processpath == "")
    {
        writeMessage(RED_TEXT_HTML, "Process Path is incorrectly configured.");
        SetupComplete = false;
    }
    
    // check video directory
    QString viddir(mConfigUi->VideoDirectory->text());
   // dir.setCurrent(viddir);
    dir.cd(viddir);
    qDebug() << viddir;
    if(viddir.isEmpty() ||viddir == "" || !dir.exists())
    {
        writeMessage(RED_TEXT_HTML, "Video Directory is incorrectly configured.");
        SetupComplete = false;
    }

    QString device(mConfigUi->SelectDevice->currentText());
    if(device.isEmpty() ||device == "" )
    {
        writeMessage(RED_TEXT_HTML, "Could Not Find Camera Device!");
        SetupComplete = false;

        this->close();
        qWarning() << "No Camera was found.";
        mConfigDialog.close();
        mApp->exit(2);
        return false;
    }


    QString resolution(mConfigUi->SelectResolution->currentText());
    if(resolution.isEmpty() ||resolution == "" )
    {
        writeMessage(RED_TEXT_HTML, "No Resolution Specified");
        SetupComplete = false;
    }


    QString format(mConfigUi->SelectInputFormat->currentText());
    if(format.isEmpty() ||format == "" )
    {
        writeMessage(RED_TEXT_HTML, "Invalid Input Format Specified.");
        SetupComplete = false;
    }

    Qt::CheckState state = mConfigUi->ToggleAutonomousMode->checkState();
    mAutonomousMode =  state == Qt::Checked ? true:false;



    QString encoder(mConfigUi->SelectVideoEncoder->currentText());
    if(encoder.isEmpty() ||encoder == "" )
    {
        writeMessage(RED_TEXT_HTML, "No Video Encoder Specified");
        SetupComplete = false;
    }
    mTrayIcon->showMessage("Passed Configuration","Configuration Passed please choose a file location!");
    return SetupComplete;
}


void MainWindow::configureVideoRecordingManager()
{
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


void MainWindow::writeMediaInfo(QString video_path, QString mediadata)
{
    writeMessage(BLUE_TEXT_HTML, QString("Finshed Recording Video: " + video_path));
    writeMessage(GREEN_TEXT_HTML, QString(mediadata).toHtmlEscaped());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mShowMain_action;
    delete mShowCfg_action;
    delete mQuit_action;
    delete mTrayIcon;
}
