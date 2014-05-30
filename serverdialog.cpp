#include "serverdialog.h"
#include "ui_serverdialog.h"
#include <QTableWidgetItem>
#include <QLabel>
ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);

    // add status bar
    mStatusText = new QLabel(ui->statusBar);
    mStatusText->setText("Server not connected");
    mStatusText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    mStatusText->setGeometry(0,0,200,40);
    mStatusText->show();
    ui->statusBar->addWidget(mStatusText);
    ui->statusBar->show();

}


void ServerDialog::connectionCreated(QString socket_id)
{
    // if the socket existed before, updtate its state else add a new item with state connected
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * socket_id_item = socket_id_items[0];
        QTableWidgetItem * status_item = ui->tableWidget->item(socket_id_item->row(),1);
        status_item->setText("Connected");
    }
    else
    {
        QTableWidgetItem * socket_id_item = new QTableWidgetItem(socket_id);
        QTableWidgetItem * status_item = new QTableWidgetItem("Connected");
        int row_count(ui->tableWidget->rowCount());
        ui->tableWidget->insertRow(row_count);
        ui->tableWidget->setItem(row_count,0,socket_id_item);
        ui->tableWidget->setItem(row_count,1,status_item);
    }

}

void ServerDialog::connectionErrorOccured(QString socket_id, QString error_message)
{
    // update the connection status
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * socket_id_item = socket_id_items[0];
        QTableWidgetItem * status_item = ui->tableWidget->item(socket_id_item->row(),1);
        status_item->setText(error_message);
    }
}

void ServerDialog::connectionStateChanged(QString socket_id, QString state_message)
{
    // update the associate socket state
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * socket_id_item = socket_id_items[0];
        QTableWidgetItem * state_item = ui->tableWidget->item(socket_id_item->row(),1);
        state_item->setText(state_message);
    }
}


void ServerDialog::socketDisconnected(QString socket_id)
{
    // update the associated socket status to disconnected
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * item = socket_id_items[0];
        QTableWidgetItem * status = ui->tableWidget->item(item->row(),1);
        status->setText("Disconnected");
    }
}

void ServerDialog::serverListening(QHostAddress address, int port)
{
    // update the server status string in the Status bar
    QString server_status("Server Running:");
    server_status.append(" @ " + address.toString());
    server_status.append(" on " + QString::number(port) );
    mStatusText->setText(server_status);

}

ServerDialog::~ServerDialog()
{
    delete mStatusText;
    delete ui;    
}
