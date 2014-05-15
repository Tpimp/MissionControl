#include "videorecordingmanager.h"
#include <QDebug>
VideoRecordingManager::VideoRecordingManager(QObject *parent,
                                             QString video_directory,
                                             QString video_process_path,
                                             QString video_encoder,
                                             QString input_format,
                                             QString recording_resolution,
                                             QString current_device) :
    QObject(parent),mCurrentDirectory(video_directory),
    mInfoReader(this,"mediainfo"),
    mVideoWriterProcess(this, video_process_path,video_encoder,
                        input_format,recording_resolution,
                        current_device)
{
    connect(&mVideoWriterProcess,SIGNAL(finished(int,QProcess::ExitStatus)),
            this,SLOT(recordingFinished(int,QProcess::ExitStatus)));
    connect(&mInfoReader,SIGNAL(FetchedMediaInfo(QString,QString)),this,SLOT(recievedMediaInfo(QString,QString)));
}



void VideoRecordingManager::recievedMediaInfo(QString video_path, QString mediadata)
{
    emit mediaInfoAvailable(video_path,mediadata);
}


void VideoRecordingManager::recordingFinished(int,QProcess::ExitStatus)
{
    mInfoReader.FetchMediaInfo(mCurrentDirectory + mCurrentVideoName);
}


void VideoRecordingManager::startRecording(QString filename)
{
    if(mVideoWriterProcess.state() == QProcess::Running)
        mVideoWriterProcess.stopRecording();
    mCurrentVideoName = filename;
    mVideoWriterProcess.startRecording(mCurrentDirectory + filename);
}


void VideoRecordingManager::requestMediaInfo(QString video_path)
{
    mInfoReader.FetchMediaInfo(video_path);
}


void VideoRecordingManager::stopRecording()
{
    mVideoWriterProcess.stopRecording();
}


VideoRecordingManager::~VideoRecordingManager()
{
    mVideoWriterProcess.stopRecording();
}
