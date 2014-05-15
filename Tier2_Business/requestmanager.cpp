#include "requestmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QTcpSocket>
#include <QStringList>

#include "Tier3_Persistance/videolocator.h"

RequestManager::RequestManager(QObject *parent) :
    QObject(parent)
{

}

bool RequestManager::addWatchList(QTcpSocket *socket)
{
    bool added_to_request_buffer;
    if(!(added_to_request_buffer = mRequestBuffer.contains(socket)))
    {
        mRequestBuffer[socket] = QByteArray();
        connect(socket, SIGNAL(readyRead()),this,SLOT(dataReadyForBuffering()));
        connect(socket, SIGNAL(readChannelFinished()),this,SLOT(dataFinishedBuffering()));
    }
    return added_to_request_buffer;
}


void RequestManager::dataFinishedBuffering()
{
    // get a pointer to the calling socket
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());
    // get the buffered message associated
    QByteArray current_buffered(mRequestBuffer[socket]);
    // get the end of the message
    current_buffered.append(socket->readAll());

    // remove the socket and disconnect its from the RequestManager slots
    mRequestBuffer.remove(socket);
    disconnect(socket, SIGNAL(readyRead()),this,SLOT(dataReadyForBuffering()));
    disconnect(socket, SIGNAL(readChannelFinished()),this,SLOT(dataFinishedBuffering()));
    qDebug() << "Finishing Message" << current_buffered;
    if(current_buffered.size() > 4)
        // Process the buffered message
        processMessage(current_buffered);
}


void RequestManager::dataReadyForBuffering()
{
    // get a pointer to the calling socket
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());
    // get the buffered message associated
    QByteArray current_buffered(mRequestBuffer[socket]);
    // get the end of the message
    int index = -1;
    current_buffered.append(socket->readAll());
    if( (index = current_buffered.lastIndexOf('\004')) )
    {
        processMessage(current_buffered.left(index));
        current_buffered.remove(0,index+1);
    }

    // set the new buffered data and return
    mRequestBuffer[socket] = current_buffered;
}



QVariant RequestManager::processMessage(QString buffered_message)
{
    // Parse function method and parameters
    buffered_message.chop(1);
    buffered_message.remove(0,1);
    // First turn string into QJSonDocument
    QJsonParseError error;
    QJsonDocument document( QJsonDocument::fromJson(buffered_message.toLocal8Bit(),&error));
    // Check if there was an error
    if(error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error Parsing incoming Json Message:" << error.errorString() << "\nMessage Recieved: " << buffered_message;
        return QVariant();
    }

    // Begin Parsing Arguments
    QJsonArray request_array (document.array());
    QVariant parameter;
    QJsonObject func = request_array.first().toObject();
    int param_count = request_array.size()-1;
    if(param_count > 1)
    {
        QVariantList params;
        for(int i = 1; i < param_count+1; ++i)
        {
            params.append(request_array.at(i).toObject());
        }
        parameter.setValue(params);
    }
    else if( param_count == 1)
    {
        parameter = QVariant::fromValue(request_array.at(param_count).toObject());
    }
    else
    {
        parameter = QVariant();
    }
    QString functionname(func["FuncName"].toString());
    QTcpSocket * caller(qobject_cast<QTcpSocket*>(sender()));
    QVariant return_val = processRequest(functionname,caller,parameter);
    if(!return_val.isNull())
    {
        QJsonObject obj = return_val.value<QJsonObject>();
        QJsonDocument doc;
        doc.setObject(obj);
        QByteArray message(doc.toJson());
        return_val = QVariant::fromValue(doc);
        message.push_front("\001");
        message.push_back("\004");
        emit returnMessageReady(caller,message);
    }

    return return_val;
}


QVariant RequestManager::processRequest(QString function, QTcpSocket *caller, QVariant params)
{
    QVariant variant;
    if(function.isEmpty())
        return QVariant();
    if(function.at(0).toLatin1() == 'V')
    {
        if(function.at(1).toLatin1() == 'i')
        {
            if(function.at(5) == 'L')
            {
                variant = requestVideoList(caller,params);
            }
            else if(function.at(5) == 'I')
            {
                variant = requestVideoList(caller, params);
            }
            else if(function.at(5) == 'T')
            {
                variant = requestVideoTransfer(caller,params);
            }
        }
        else
        {

        }
    }
    else
    {

    }

    return variant;
}


bool RequestManager::removeWatchList(QTcpSocket *socket)
{
    bool removed = false;
    if(mRequestBuffer.contains(socket))
    {
        removed = mRequestBuffer.remove(socket);
    }
    return removed;
}


QVariant RequestManager::requestVideoList(QTcpSocket *caller, QVariant params)
{
    QStringList videos;
    int max_return = -1;
    if(!params.isNull())
    {
        // Get the packed max_return value
        QJsonObject max_return_obj(params.toJsonObject());
        QJsonValue max_ret_val( max_return_obj["max_return"]);
        QVariant max_ret_var(max_ret_val.toVariant());
        if(max_ret_var.type() == QVariant::Int)
            max_return = max_ret_var.toInt();
    }
    videos = VideoLocator::findVideos(mVideoDirectory);
    if(max_return != -1)
    {
        int amount_to_remove = videos.length()-max_return;
        //only return the amount passed in
        for(int i = 0; i < amount_to_remove; ++i)
        {
            videos.pop_back();
        }
    }

    QJsonObject ret_object;
    ret_object["VideoList"] = QJsonValue::fromVariant(QVariant::fromValue(videos));
    QVariant ret_var(QVariant::fromValue(ret_object));
    qDebug() << "Sending back" << ret_var;
    return ret_var;

}


QVariant RequestManager::requestVideoInfo(QTcpSocket *caller, QVariant params)
{
    return QVariant();
}

QVariant RequestManager::requestVideoTransfer(QTcpSocket *caller, QVariant params)
{
    return QVariant();
}

RequestManager::~RequestManager()
{

}
