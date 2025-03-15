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

void ChatDialog::on_searchLE_textEdited(const QString &arg1)
{
    ui->clearPB->show();
}


void ChatDialog::on_clearPB_clicked()
{
    ui->searchLE->clear();
}



void ChatDialog::on_topPB_toggled(bool checked)
{
    auto pb=ui->topPB;
    if(checked){
        pb->setIcon(QIcon(":/img/loginDialog/img/top_clicked.svg"));
    }else{
        pb->setIcon(QIcon(":/img/loginDialog/img/top.svg"));
    }
}


void ChatDialog::on_maxPB_toggled(bool checked)
{
    auto pb=ui->maxPB;
    if(checked){
        pb->setIcon(QIcon(":/img/loginDialog/img/max_clicked.svg"));
    }else{
        pb->setIcon(QIcon(":/img/loginDialog/img/max.svg"));
    }
}

