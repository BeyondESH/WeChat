#include "chatdialog.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}


void ChatDialog::on_searchLE_editingFinished()
{
    ui->quitPB->close();
}


void ChatDialog::on_searchLE_textEdited(const QString &arg1)
{
    ui->quitPB->show();
}

