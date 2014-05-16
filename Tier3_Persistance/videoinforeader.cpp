#include "videoinforeader.h"
#include <QDebug>
VideoInfoReader::VideoInfoReader(QObject *parent, QString media_info_process) :
    QObject(parent), mProcessPath(media_info_process), mProcess(this), mMediaData("")
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
        args << video_path;
        qDebug() << video_path;
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
    emit FetchedMediaInfo(mCurrentMediaPath,mMediaData);
}

void VideoInfoReader::RecievedBytes()
{
    // grab the bytes ready
    mMediaData.append(mProcess.readAll());
}


