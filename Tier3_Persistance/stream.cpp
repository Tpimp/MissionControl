#include "stream.h"
Stream::Stream(QObject *parent, QIODevice *io, QAbstractSocket *& socket) :
    QObject(parent), mBuffer(this), mSocket(*socket), mFile(nullptr)
{
    mStream.setDevice(io);
    if(!io->isOpen())
        io->open(QIODevice::ReadOnly);
    connect(&mSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
    char *buff = new char[MAX_MESSAGE];
    uint size(MAX_MESSAGE);
    mStream.readBytes(buff,size);
    mBuffer.write(buff,size);
    mSocket.write(mBuffer.data().data(), size);
}

Stream::Stream(QObject *parent, QString file_path, QAbstractSocket *socket) :
    QObject(parent), mBuffer(this), mSocket(*socket)
{
    mFile = new QFile(file_path,this);
    mStream.setDevice(mFile);
    mFile->open(QIODevice::ReadOnly);
    connect(&mSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
    mSocket.write(mBuffer.data().data(), MAX_MESSAGE);
}



void Stream::bytesWritten(qint64 bytes_wrote)
{
    if(!mStream.atEnd())
    {
        uint bytes_written(bytes_wrote);
        char *buff = new char[bytes_written];

        mStream.readBytes(buff,bytes_written);
        bytes_wrote = mBuffer.write(buff, bytes_wrote);
        mSocket.write(mBuffer.data().data(), bytes_wrote);
        delete buff;
    }
    emit updateTotalBytesWritten(bytes_wrote);
}


Stream::~Stream()
{
    mBuffer.close();
    mStream.device()->close();
    mSocket.close();
    delete mFile;

}
