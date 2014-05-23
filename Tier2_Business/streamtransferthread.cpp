#include "streamtransferthread.h"

StreamTransferThread::StreamTransferThread(QObject *parent, QWidget *&mainwindow,  QString &filepath, QAbstractSocket *&socket, qint64& stream_size) :
    QThread(parent),mStream(this, filepath, socket), mProgress(mainwindow)
{
    connect(&mStream,
            SIGNAL(updateTotalBytesWritten(qint64)),
            &mProgress,
            SLOT(updateProgress(qint64)));
}


void StreamTransferThread::run()
{

    mProgress.show();
    exec();

    emit finishedTransferingStream(mStream.getFileName());

}
