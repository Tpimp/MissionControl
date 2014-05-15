#include "serverdialog.h"
#include "ui_serverdialog.h"
#include <QTableWidgetItem>
#include <QLabel>
ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
}


void ServerDialog::connectionCreated(QString socket_id)
{
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * item = socket_id_items[0];
        QTableWidgetItem * status = ui->tableWidget->item(item->row(),1);
        status->setText("Connected");
    }
    else
    {
        QTableWidgetItem * socket_id_item = new QTableWidgetItem(socket_id);
        QTableWidgetItem * state = new QTableWidgetItem("Connected");
        int rows(ui->tableWidget->rowCount());
        ui->tableWidget->insertRow(rows);
        ui->tableWidget->setItem(rows,0,socket_id_item);
        ui->tableWidget->setItem(rows,1,state);
    }

}

void ServerDialog::connectionErrorOccured(QString socket_id, QString error_message)
{
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * item = socket_id_items[0];
        QTableWidgetItem * status = ui->tableWidget->item(item->row(),1);
        status->setText(error_message);
    }
}

void ServerDialog::connectionStateChanged(QString socket_id, QString state_message)
{
    QList<QTableWidgetItem *> socket_id_items = ui->tableWidget->findItems(socket_id,Qt::MatchExactly);
    if(socket_id_items.size() > 0)
    {
        QTableWidgetItem * item = socket_id_items[0];
        QTableWidgetItem * status = ui->tableWidget->item(item->row(),1);
        status->setText(state_message);
    }
}


void ServerDialog::socketDisconnected(QString socket_id)
{
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
    QString msg("Server Started:");
    msg.append(" @ " + address.toString());
    msg.append(" on " + QString::number(port) );
    QLabel * status_label = new QLabel(ui->statusBar);
    status_label->setText(msg);
    status_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    status_label->setGeometry(0,0,200,40);
    status_label->show();
    ui->statusBar->addWidget(status_label);
    ui->statusBar->show();
}

ServerDialog::~ServerDialog()
{
    delete ui;
}
