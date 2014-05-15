#ifndef VIDEOWRITERPROCESS_H
#define VIDEOWRITERPROCESS_H

#include <QProcess>

class VideoWriterProcess : public QProcess
{
    Q_OBJECT
public:
    explicit VideoWriterProcess(QObject *parent, QString video_process_path, QString video_encoder, QString input_format,
                                QString recording_resolution, QString current_device);
    ~VideoWriterProcess();

    QString mVideoProcess;
    QString mVideoEncoder;
    QString mInputFormat;
    QString mRecordingResolution;
    QString mCurrentDevice;

public slots:

    void startRecording(QString filename);
    void stopRecording();

};

#endif // VIDEOWRITERPROCESS_H
