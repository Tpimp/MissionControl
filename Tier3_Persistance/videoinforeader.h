#ifndef VIDEOINFOREADER_H
#define VIDEOINFOREADER_H

#include <QObject>
#include <QProcess>
#include <QPixmap>
#include <QBuffer>

class VideoInfoReader : public QObject
{
    Q_OBJECT
public:
    explicit VideoInfoReader(QObject *parent, QString media_info_process, QString file_format);
    ~VideoInfoReader();
signals:
    void fetchedMediaInfo(QString video_name, QString video_info);

public slots:
    void fetchMediaInfo(QString video_path);
    void processFinished();
    void thumbnailProcessFinished();
    void recievedBytes();

private:
    void fetchVideoThumbnail();
    QString  mMediaData;
    QString  mFormatFile = R"(/home/odroid/.config/MissionControl/media_info_format.txt)";
    QString  mCurrentMediaPath;
    QString  mProcessPath;
    QProcess* mProcess;
    QByteArray mThumbnailBuffer;



};

#endif // VIDEOINFOREADER_H
