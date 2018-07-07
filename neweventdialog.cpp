#include "neweventdialog.h"
#include "ui_neweventdialog.h"

NewEventDialog::NewEventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEventDialog)
{
    ui->setupUi(this);
}

NewEventDialog::~NewEventDialog()
{
    delete ui;
}

QString NewEventDialog::getName()
{
    return ui->leName->text();
}

QString NewEventDialog::getParams()
{
    return ui->leParams->toPlainText();
}

void NewEventDialog::setName(QString s)
{
    ui->leName->setText(s);
}

void NewEventDialog::setParams(QString s)
{
    ui->leParams->setText(s);
}

void NewEventDialog::on_buttonBox_accepted()
{
    accept();
}

void NewEventDialog::on_buttonBox_rejected()
{
    reject();
}
