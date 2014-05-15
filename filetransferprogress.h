#ifndef FILETRANSFERPROGRESS_H
#define FILETRANSFERPROGRESS_H

#include <QWidget>

namespace Ui {
class FileTransferProgress;
}

class FileTransferProgress : public QWidget
{
    Q_OBJECT

public:
    explicit FileTransferProgress(QWidget *parent = 0, qint64 size = 0);
    ~FileTransferProgress();
    Ui::FileTransferProgress *ui;

public slots:
    void updateProgress(qint64 bytes_transfered);

private:
    qint64 mBytesSent = 0;
    qint64 mTotalBytes;
};

#endif // FILETRANSFERPROGRESS_H
