#ifndef VIDEORECORDINGMANAGER_H
#define VIDEORECORDINGMANAGER_H

#include <QObject>
#include "Tier3_Persistance/videoinforeader.h"
#include "Tier3_Persistance/videowriterprocess.h"

class MainWindow;

class VideoRecordingManager : public QObject
{
    Q_OBJECT
public:
    explicit VideoRecordingManager(MainWindow *parent, QString video_directory, QString video_process_path,
                                   QString video_encoder, QString input_format,
                                   QString recording_resolution,
                                   QString current_device);
    ~VideoRecordingManager();
    QString     mCurrentDirectory;

    VideoInfoReader * getVideoInfoReader(){return &mInfoReader;}

signals:

private slots:
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
