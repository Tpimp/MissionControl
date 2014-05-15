#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
     Ui::ConfigDialog *ui;
     
public slots:
     void openDialog();
     void findProcPath();
     void findVidDir();

};

#endif // CONFIGDIALOG_H
