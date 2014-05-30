#include "videorecordingmanager.h"

#include <QDebug>

#include "mainwindow.h"

VideoRecordingManager::VideoRecordingManager(MainWindow *parent,
                                             QString video_directory,
                                             QString video_process_path,
                                             QString video_encoder,
                                             QString input_format,
                                             QString recording_resolution,
                                             QString current_device) :
    QObject(parent),mCurrentDirectory(video_directory),
    mInfoReader(this,"mediainfo", "//home/odroid/.config/MissionControl/media_info_format.txt"),
    mVideoWriterProcess(this, video_process_path,video_encoder,
                        input_format,recording_resolution,
                        current_device)
{
    connect(&mVideoWriterProcess,SIGNAL(finished(int,QProcess::ExitStatus)),
            this,SLOT(recordingFinished(int,QProcess::ExitStatus)));

    connect(&mInfoReader,SIGNAL(FetchedMediaInfo(QString,QString)),parent,
            SLOT(writeMediaInfo(QString,QString)));


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
