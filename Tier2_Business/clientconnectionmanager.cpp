#include "clientconnectionmanager.h"

ClientConnectionManager::ClientConnectionManager(QObject *parent) :
    QObject(parent), mServer(this), mRequestMan(this)
{
    connect(&mServer,SIGNAL(newConnection()),this,SLOT(incomingConnection()));
    connect(&mRequestMan,SIGNAL(returnMessageReady(QTcpSocket*,QByteArray)),
            this,SLOT(sendReturnMessage(QTcpSocket*,QByteArray)));
}

void ClientConnectionManager::incomingConnection()
{
    QTcpSocket * connection;
    while((connection = mServer.nextPendingConnection()) != nullptr)
    {
        mSockets.append(connection);
        mRequestMan.addWatchList(connection);
        QString socket_id = QString::number(int(connection->socketDescriptor()));
        emit connectionCreated(socket_id);
        connect(connection,SIGNAL(disconnected()),this,SLOT(socketDisconnected()));
        connect(connection,
                SIGNAL(error(QAbstractSocket::SocketError)),
                this,
                SLOT(connectionErrorOccured(QAbstractSocket::SocketError)));
        connect(connection,
                SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this,
                SLOT(connectionStateChanged(QAbstractSocket::SocketState)));
    }
}

void ClientConnectionManager::connectionErrorOccured(QAbstractSocket::SocketError socketError)
{
    QTcpSocket * socket = qobject_cast<QTcpSocket *>(sender());
    QString error_message;
    error_message = socket->errorString();

    // get the socket id
    QString socket_id = QString::number(int(socket->socketDescriptor()));
    emit connectionErrored(socket_id, error_message);
}



void ClientConnectionManager::connectionStateChanged(QAbstractSocket::SocketState socketState)
{
    QString state;
    switch(socketState)
    {
        case(QAbstractSocket::UnconnectedState):
        {
            state = "The socket is not connected.";
            break;
        }
        case(QAbstractSocket::HostLookupState):
        {
            state = "The socket is performing a host name lookup.";
            break;
        }
        case(QAbstractSocket::ConnectingState):
        {
            state = "The socket has started establishing a connection.";
            break;
        }
        case(QAbstractSocket::ConnectedState):
        {
            state = "A connection is established.";
            break;
        }
        case(QAbstractSocket::BoundState):
        {
            state = "The socket is bound to an address and port.";
            break;
        }
        case(QAbstractSocket::ClosingState):
        {
            state = "The socket is about to close (data may still be waiting to be written).";
            break;
        }
        case(QAbstractSocket::ListeningState):
        {
            state = "For internal use only.";
            break;
        }

    }

    // get a pointer to the calling socket
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());

    // get the socket id
    QString socket_id = QString::number(int(socket->socketDescriptor()));
    emit connectionChangedState(socket_id, state);

}


void ClientConnectionManager::sendReturnMessage(QTcpSocket * target, QByteArray message)
{
    qDebug() << "Sending Return Message: " << message;
    target->write(message.data(),message.size());
    target->waitForBytesWritten(3000);
}


void ClientConnectionManager::socketDisconnected()
{
    // get a pointer to the calling socket
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());

    QString socket_id = QString::number(int(socket->socketDescriptor()));

    emit connectionLost(socket_id);
    disconnect(socket,0,0,0);

    // get the socket id

    // clean the socket up
    int removed = mSockets.removeAll(socket);

    if(removed > 1 )
        qDebug() << "More than one removed?";


}


void ClientConnectionManager::startServer(QHostAddress address, int port)
{
    mServer.listen(address,port);
    emit serverBeganListening(mServer.serverAddress(),mServer.serverPort());
}

