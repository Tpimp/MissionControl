#ifndef CLIENTCONNECTIONMANAGER_H
#define CLIENTCONNECTIONMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "Tier2_Business/requestmanager.h"
#include "Tier2_Business/videorecordingmanager.h"

class ClientConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnectionManager(QObject *parent = 0);
    void startServer(QHostAddress address = QHostAddress::AnyIPv4, int port = 8889);

signals:
    void connectionCreated(QString socket_id);
    void connectionErrored(QString socket_id, QString error_message);
    void connectionLost(QString socket_id);
    void connectionChangedState(QString socket_id, QString state);
    void serverBeganListening(QHostAddress serveraddress, int port);

private slots:
    void connectionErrorOccured(QAbstractSocket::SocketError socketError);
    void connectionStateChanged(QAbstractSocket::SocketState socketState);
    void sendReturnMessage(QTcpSocket * target, QByteArray message);
    void incomingConnection();
    void socketDisconnected();

private:
    QTcpServer             mServer;
    QList<QTcpSocket*>     mSockets;
    RequestManager         mRequestMan;
};

#endif // CLIENTCONNECTIONMANAGER_H
