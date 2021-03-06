#include "videowriterprocess.h"
#include <QDebug>
VideoWriterProcess::VideoWriterProcess(QObject *parent,
                                       QString video_process_path,
                                       QString video_encoder,
                                       QString input_format,
                                       QString recording_resolution,
                                       QString current_device)
                                       /* End Parameters */:
    // Base Member Initialization
    QProcess(parent), mVideoProcess(video_process_path),
    mVideoEncoder(video_encoder), mInputFormat(input_format),
    mRecordingResolution(recording_resolution),
    mCurrentDevice(current_device)
{}


void VideoWriterProcess::startRecording(QString filename)
{
    // Create arguments list and fill it
    QStringList arguments;
    // set the video encoder codec to use
    arguments << "-f"  << mVideoEncoder
                       // set input format
                       << "-r" << "25"
                       <<  "-i" << mCurrentDevice
                      // << "-input_format" << mInputFormat
                       << "-strict" << "experimental"
                       // set the recording resolution
                       << "-s" << mRecordingResolution
                       << "-vcodec" << "mpeg4"
                       // set filename
                       << "-y" << (filename);
    qDebug() << "Recording arguments" << arguments;
    // Begin the video writer sub process
    start(mVideoProcess,arguments);
}

void VideoWriterProcess::stopRecording()
{
   close();
}

VideoWriterProcess::~VideoWriterProcess()
{
    stopRecording();
}
