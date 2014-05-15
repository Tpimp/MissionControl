#include "filetransferprogress.h"
#include "ui_filetransferprogress.h"

FileTransferProgress::FileTransferProgress(QWidget *parent, qint64 size) :
    QWidget(parent), mTotalBytes(size),
    ui(new Ui::FileTransferProgress)
{
    ui->setupUi(this);
    ui->TransferProgress->setRange(0,size);
    ui->TransferProgress->setValue(0);
}

FileTransferProgress::~FileTransferProgress()
{
    delete ui;
}


void FileTransferProgress::updateProgress(qint64 bytes_transfered)
{
    mBytesSent += bytes_transfered;
    ui->TransferProgress->setValue(mBytesSent);
}
