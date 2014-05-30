#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <QLabel>
namespace Ui {
class ServerDialog;
}

class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = 0);
    Ui::ServerDialog *ui;
    ~ServerDialog();

private slots:

    void connectionCreated(QString socket_id);
    void connectionErrorOccured(QString socket_id,QString error_message);
    void connectionStateChanged(QString socket_id,QString state_message);
    void socketDisconnected(QString socket_id);
    void serverListening(QHostAddress address, int port);

private:
    QLabel*    mStatusText;
};

#endif // MAINWINDOW_H
