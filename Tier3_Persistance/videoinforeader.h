#ifndef VIDEOINFOREADER_H
#define VIDEOINFOREADER_H

#include <QObject>
#include <QProcess>
class VideoInfoReader : public QObject
{
    Q_OBJECT
public:
    explicit VideoInfoReader(QObject *parent, QString media_info_process);

signals:
    void FetchedMediaInfo(QString video_name, QString video_info);


public slots:

    void FetchMediaInfo(QString video_path);
    void ProcessFinished();
    void RecievedBytes();

private:
    QString mMediaData;
    QString mCurrentMediaPath;
    QString mProcessPath;
    QProcess mProcess;



};

#endif // VIDEOINFOREADER_H
