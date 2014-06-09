#include "requestmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QStringList>
#include <QThreadPool>
#include "Tier3_Persistance/videolocator.h"
#include "Tier3_Persistance/videoinforeader.h"
#include <QPixmap>
#include "Tier2_Business/streamtransferthread.h"
#include <QMainWindow>

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


QVariant RequestManager::processRequest(QString function, QTcpSocket * caller, QVariant params)
{
    QVariant variant;
    if(function.isEmpty())
        return QVariant();

    // Create parse branch RPC implementation

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
                variant = requestVideoInfo(caller, params);
            }
            else if(function.at(5) == 'T')
            {
                variant = requestFileTransfer(caller,params);
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
    //qDebug() << "Sending back" << ret_var;
    return ret_var;

}


QVariant RequestManager::requestVideoInfo(QTcpSocket *caller, QVariant params)
{
    // Unpack the QVariant
    // Layers - QJsonObject { [ QJSonValue FuncName = "VideoInfo", QJSonValue  VideoName = "video_passed"] }
    QJsonObject jobject = params.toJsonObject();
    QJsonValue videoname_value = jobject["VideoName"];
    QString videoname;
    if(videoname_value.isString())
        videoname = videoname_value.toString();
    else
    {
        qDebug() << "VideoInfo function passed invalid paramter VideoName: " << videoname_value;
        return QVariant();
    }
    // fetched videoname now check if the info file exists

    //TODO Replace hard coded path with application settings manager variable
    VideoInfoReader   * inforeader = new VideoInfoReader(this,"mediainfo", "//home/odroid/.config/MissionControl/media_info_format.txt");


    // work some lambda magic Connect inforeader to the RequestManager,
    // when the media is fetched... call an inline "Slot" and pass name and info
    connect(inforeader, &VideoInfoReader::fetchedMediaInfo , [=](QString videoname, QString videoinfo)
    {   // ... execute this inline lamdba function

        // disconnect the slot (don't need it anymore clean up)
        disconnect(inforeader, 0, this, 0);


        QJsonObject return_obj;
        return_obj["VideoName"] = videoname.remove("/home/odroid/Videos//");
        return_obj["VideoInfo"] = videoinfo;
        // create a JsonDocument Wrapper = "{}"
        QJsonDocument doc;
        doc.setObject(return_obj);
        QByteArray message(doc.toJson()); // out to JSON
        // Prepend the Start Byte and Append the End Byte
        message.push_front("\001");
        message.push_back("\004");
        inforeader->deleteLater();
        emit returnMessageReady(caller, message);
    });

    inforeader->fetchMediaInfo(mVideoDirectory + "//" + videoname);
    return QVariant();
}





QVariant RequestManager::requestFileTransfer(QTcpSocket *& caller, QVariant params)
{
    // VIDEO TRANSFER COMING SOON
    // get the video name
    QJsonObject jobject = params.toJsonObject();
    QJsonValue file_name_value = jobject["FileName"];
    QString file_name;
    if(file_name_value.isString())
        file_name = file_name_value.toString("");
    else
    {
        qDebug() << "VideoTransfer function passed invalid paramter VideoName: " << file_name_value;
        return QVariant();
    }
    QFile file_in(file_name,this);
    qint64 file_size = file_in.size();
    qDebug() << "Preparing to send file: " << file_name << " of size " << file_size;
    //get pointer to main ui object
    QWidget * mainwindow = qobject_cast<QWidget *>(parent()->parent());
    // spwan off stream transfer video
    QAbstractSocket * caller_socket = qobject_cast< QAbstractSocket *>(caller);
    StreamTransferThread * file_transfer = new StreamTransferThread(qobject_cast<QObject *>(this),mainwindow,
                                                                  file_name,caller_socket,file_size);
    connect(file_transfer, &StreamTransferThread::finishedTransferingStream,
            this,       &RequestManager::videoTransferThreadFinished);
    file_transfer->start();
    return QVariant();

}


void RequestManager::videoTransferThreadFinished(QString videoname)
{
    qDebug() << "Finished Sending: " << videoname;
    disconnect(sender(),0,this,0);
    sender()->deleteLater();


}

RequestManager::~RequestManager()
{

}
