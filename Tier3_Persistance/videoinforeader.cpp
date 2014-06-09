#include "videoinforeader.h"
#include <QDebug>
#include <QFile>

VideoInfoReader::VideoInfoReader(QObject *parent, QString media_info_process, QString file_format) :
    QObject(parent), mProcessPath(media_info_process), mProcess(nullptr), mMediaData(""),mFormatFile(file_format)
{}


void VideoInfoReader::fetchMediaInfo(QString video_path)
{
    mProcess = new QProcess(this);
    connect(mProcess,SIGNAL(readyRead()),this,SLOT(recievedBytes()));
    connect(mProcess,SIGNAL(finished(int)),this,SLOT(processFinished()));
    // Prepare arguments list
    QStringList args;
    //qDebug() << mFormatFile;
    args << R"(--Inform=file:/)" + mFormatFile;
    args << video_path;
    //qDebug() << args;
    // set current path
    mCurrentMediaPath = video_path;
    // clear the current data
    mMediaData.clear();
    // start the process
    mProcess->start(mProcessPath,args);

}


void VideoInfoReader::fetchVideoThumbnail()
{
    mProcess = new QProcess(this);
        // Prepare arguments list
    QStringList args;
    args << R"(-i)";
    args << mCurrentMediaPath;
    args << R"(-t)";
    args << "1";
    args << R"(-s)";
    args << R"(1280x720)";
    args << R"(-o)";
    args << mCurrentMediaPath + R"(.png)";
    //qDebug() << args;
    // set current path
    // start the process
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(thumbnailProcessFinished()));
    mProcessPath = "ffmpegthumbnailer";
    mProcess->open();
    mProcess->start(mProcessPath,args);

}

void VideoInfoReader::processFinished()
{
    // notify top layers

    // Replace mediainfo string size with a helpful size format
    QFile video(mCurrentMediaPath);
    qint64 size(video.size());
    QString replace_str("File size                                : ");
    replace_str.append(QString::number(size));
    replace_str.append("\n");
    mMediaData.replace("File size*:*\n", replace_str);
    mProcess->terminate();
    mProcess->close();
    mProcess->deleteLater();
    emit fetchedMediaInfo(mCurrentMediaPath,mMediaData);
    //fetchVideoThumbnail();
}



void VideoInfoReader::thumbnailProcessFinished()
{

   // qDebug() << mCurrentMediaPath+".png";

   // emit fetchedMediaInfo(mCurrentMediaPath,mMediaData,);mCurrentMediaPath+".png");

}

void VideoInfoReader::recievedBytes()
{
    // grab the bytes ready
    mMediaData.append(mProcess->readAll());
}

VideoInfoReader::~VideoInfoReader()
{

}
