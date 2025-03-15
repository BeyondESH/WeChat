#include "chatdialog.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    init();
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::on_searchLE_textEdited(const QString &arg1)
{
    ui->clearPB->show();
    ui->chatUserList->close();
    ui->conUserList->close();
    ui->searchList->show();
}

void ChatDialog::on_clearPB_clicked()
{
    ui->searchLE->clear();
    ui->chatUserList->show();
    ui->conUserList->close();
    ui->searchList->close();
}

void ChatDialog::on_topPB_toggled(bool checked)
{
    auto pb = ui->topPB;
    if (checked)
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/top_clicked.svg"));
    }
    else
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/top.svg"));
    }
}

void ChatDialog::on_maxPB_toggled(bool checked)
{
    auto pb = ui->maxPB;
    if (checked)
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max_clicked.svg"));
        this->parentWidget()->showMaximized();
        qDebug() << "showMaximized";
    }
    else
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max.svg"));
        this->parentWidget()->showNormal();
        qDebug() << "showNormal";
    }
}

void ChatDialog::on_searchLE_textChanged(const QString &arg1)
{
    if (ui->searchLE->text().isEmpty())
    {
        ui->clearPB->close();
    }
    else
    {
        ui->clearPB->show();
    }
}

void ChatDialog::on_chatCB_clicked()
{
    ui->chatUserList->show();
    ui->conUserList->close();
    ui->searchList->close();
}

void ChatDialog::init()
{
    ui->clearPB->close();
    ui->chatUserList->show();
    ui->conUserList->close();
    ui->searchList->close();
}

void ChatDialog::on_usersCB_clicked()
{
    ui->chatUserList->close();
    ui->conUserList->show();
    ui->searchList->close();
}

void ChatDialog::on_quitPB_clicked()
{
    QApplication::quit();
}

void ChatDialog::on_minPB_clicked()
{
    this->parentWidget()->showMinimized();
}
