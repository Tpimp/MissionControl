#ifndef STREAM_H
#define STREAM_H

#include <QObject>
#include <QDataStream>
#include <QBuffer>
#include <QAbstractSocket>
#include <QFile>
static const uint MAX_MESSAGE = 32000;

class Stream : public QObject
{
    Q_OBJECT
public:

    explicit Stream(QObject *parent, QIODevice *io, QAbstractSocket *& socket);
    Stream(QObject *parent, QString file_path, QAbstractSocket * socket);
    ~Stream();
    QString getFileName(){return mFile ? mFile->fileName():QString();}
signals:
    void updateTotalBytesWritten(qint64 byteswritten);

public slots:

private slots:
    void bytesWritten(qint64 bytes_wrote);

private:
    QDataStream        mStream;
    QBuffer            mBuffer;
    QAbstractSocket &  mSocket;
    QFile           *  mFile = nullptr;

};

#endif // STREAM_H
