#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QHash>
#include <QTcpSocket>
#include <QDir>
#include <QStringList>
#include <QVariant>

class RequestManager : public QObject
{
    Q_OBJECT
public:
    explicit RequestManager(QObject *parent = 0);
    bool addWatchList(QTcpSocket* socket);
    bool removeWatchList(QTcpSocket* socket);
    ~RequestManager();

signals:
    void returnMessageReady(QTcpSocket * target, QByteArray message);

private slots:
    void dataFinishedBuffering();
    void dataReadyForBuffering();


private:
    QHash<QTcpSocket*,QByteArray>   mRequestBuffer;
    QVariant processMessage(QString buffered_message);
    QString                         mVideoDirectory = R"(/home/odroid/Videos)";

    // Define Requests!!!!!
    QVariant  processRequest(QString function, QTcpSocket * caller, QVariant params);
    QVariant  requestVideoInfo(QTcpSocket * caller, QVariant params);
    QVariant  requestVideoList(QTcpSocket * caller, QVariant params);
    QVariant  requestVideoTransfer(QTcpSocket * caller, QVariant params);
};

#endif // REQUESTMANAGER_H
