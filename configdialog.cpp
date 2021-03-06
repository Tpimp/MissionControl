#include "configdialog.h"
#include "ui_configdialog.h"
#include  <QFileDialog>
#include  <QCamera>
#include  <QCameraImageCapture>
#include  <QMediaRecorder>
ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    connect(ui->GetProcessPath,SIGNAL(clicked()),this, SLOT(findProcessPath()));
    connect(ui->GetVideoDirectory,SIGNAL(clicked()),this, SLOT(findVideoDirectory()));
    QList<QByteArray> camera_list( QCamera::availableDevices() );
    foreach(QByteArray device_name, camera_list)
    {
        ui->SelectDevice->addItem(device_name);
    }

    QByteArray device(ui->SelectDevice->currentText().toLocal8Bit());
    if(device != "")
    {
        // get values read from camera api
        QCamera   camera(device);
        QMediaRecorder recorder(&camera);
        QList<QSize> res_support(recorder.supportedResolutions());
        foreach(QSize size, res_support)
        {
            ui->SelectResolution->addItem(QVariant(size).toString());
        }
        QStringList containers_support(recorder.supportedContainers());
        foreach(QString container, containers_support)
        {
            ui->SelectInputFormat->addItem(container);
        }
        ui->SelectInputFormat->addItem("mjpeg");
        ui->SelectResolution->addItem(QString("1280x720"), QVariant(QSize(1280,720)));
        ui->SelectVideoEncoder->addItem("video4linux2");
    }
}


void ConfigDialog::openDialog()
{
    ui->SelectDevice->clear();
    QList<QByteArray> camera_list(QCamera::availableDevices());
    foreach(QByteArray device_name, camera_list)
    {
        ui->SelectDevice->addItem(device_name);
    }

    ui->SelectInputFormat->clear();
    ui->SelectResolution->clear();
    ui->SelectVideoEncoder->clear();
    QByteArray device(ui->SelectDevice->currentText().toLocal8Bit());
    if(device != "")
    {
        // get values read from camera api
        QCamera   camera(device);
        QMediaRecorder recorder(&camera);
        QList<QSize> res_support(recorder.supportedResolutions());
        foreach(QSize size, res_support)
        {
            ui->SelectResolution->addItem(QVariant(size).toString());
        }
        QStringList encoder_support(recorder.supportedVideoCodecs());
        foreach(QString encoder, encoder_support)
        {
            ui->SelectInputFormat->addItem(encoder);
        }
        ui->SelectInputFormat->addItem("mjpeg");
        ui->SelectResolution->addItem(QString("1280x720"), QVariant(QSize(1280,720)));
        ui->SelectVideoEncoder->addItem("video4linux2");
    }
    open();
}


void ConfigDialog::findProcessPath()
{
    // look for Process Path
    QString process_path(QFileDialog::getOpenFileName(this, "Find ffmpeg"));
    ui->ProcessPath->setText(process_path);
}

void ConfigDialog::findVideoDirectory()
{

    QString vid_dir(QFileDialog::getExistingDirectory(this, "Find Directory to Save to"));
    ui->VideoDirectory->setText(vid_dir);
}



ConfigDialog::~ConfigDialog()
{
    delete ui;
}
