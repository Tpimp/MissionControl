#include "videoinforeader.h"
#include <QDebug>
#include <QFile>

VideoInfoReader::VideoInfoReader(QObject *parent, QString media_info_process, QString file_format) :
    QObject(parent), mProcessPath(media_info_process), mProcess(this), mMediaData(""),mFormatFile(file_format)
{
    // make connections
    connect(&mProcess,SIGNAL(readyRead()),this,SLOT(RecievedBytes()));
    connect(&mProcess,SIGNAL(finished(int)),this,SLOT(ProcessFinished()));
}


void VideoInfoReader::FetchMediaInfo(QString video_path)
{
    if(!mProcess.isOpen())
    {
        // Prepare arguments list
        QStringList args;
        qDebug() << mFormatFile;
        args << R"(--Inform=file:/)" + mFormatFile;
        args << video_path;
        qDebug() << args;
        // set current path
        mCurrentMediaPath = video_path;
        // clear the current data
        mMediaData.clear();
        // start the process
        mProcess.start(mProcessPath,args);
    }
    else
         qWarning() << "Wait until last request has finished!";
}

void VideoInfoReader::ProcessFinished()
{
    // notify top layers

    // Replace mediainfo string size with a helpful size format
    QFile video(mCurrentMediaPath);
    qint64 size(video.size());
    QString replace_str("File size                                : ");
    replace_str.append(QString::number(size));
    replace_str.append("\n");
    mMediaData.replace("File size*:*\n", replace_str);

    emit FetchedMediaInfo(mCurrentMediaPath,mMediaData);
}

void VideoInfoReader::RecievedBytes()
{
    // grab the bytes ready
    mMediaData.append(mProcess.readAll());
}


