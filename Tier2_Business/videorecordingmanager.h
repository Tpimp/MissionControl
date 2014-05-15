#ifndef VIDEORECORDINGMANAGER_H
#define VIDEORECORDINGMANAGER_H

#include <QObject>
#include "Tier3_Persistance/videoinforeader.h"
#include "Tier3_Persistance/videowriterprocess.h"


class VideoRecordingManager : public QObject
{
    Q_OBJECT
public:
    explicit VideoRecordingManager(QObject *parent, QString video_directory, QString video_process_path,
                                   QString video_encoder, QString input_format,
                                   QString recording_resolution,
                                   QString current_device);
    ~VideoRecordingManager();
    QString     mCurrentDirectory;

signals:
    void  mediaInfoAvailable(QString video_path, QString mediadata);

private slots:
    void recievedMediaInfo(QString video_path, QString mediadata);
    void recordingFinished(int, QProcess::ExitStatus);

public slots:    

    void requestMediaInfo(QString video_path);

    void startRecording(QString filename);
    void stopRecording();

protected:
    VideoWriterProcess  mVideoWriterProcess;
    QString             mCurrentVideoName;
    VideoInfoReader     mInfoReader;

};

#endif // VIDEORECORDINGMANAGER_H
