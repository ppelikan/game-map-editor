#ifndef NEWEVENTDIALOG_H
#define NEWEVENTDIALOG_H

#include <QDialog>

namespace Ui {
class NewEventDialog;
}

class NewEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEventDialog(QWidget *parent = 0);
    ~NewEventDialog();

    QString getName();
    QString getParams();
    void setName(QString s);
    void setParams(QString s);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::NewEventDialog *ui;
};

#endif // NEWEVENTDIALOG_H
