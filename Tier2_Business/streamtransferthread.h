#ifndef STREAMTRANSFERTHREAD_H
#define STREAMTRANSFERTHREAD_H

#include <QThread>
#include <QAbstractSocket>
#include "Tier3_Persistance/stream.h"
#include "filetransferprogress.h"
class StreamTransferThread : public QThread
{
    Q_OBJECT
public:
    explicit StreamTransferThread(QObject *parent, QWidget*& mainwindow,  QString& filepath,
                                  QAbstractSocket *& socket, qint64& stream_stize);
    void run();
signals:
    void finishedTransferingStream(QString streamname);

public slots:

private:
    Stream  mStream;
    FileTransferProgress mProgress;
};

#endif // STREAMTRANSFERTHREAD_H
