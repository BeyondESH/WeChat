#include "chatdialog.h"
#include "ui_chatdialog.h"

#include <QMouseEvent>
ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    uiInit();
    ui->searchWidget->installEventFilter(this);
    ui->sidebarWD->installEventFilter(this);
    ui->titleWD->installEventFilter(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

bool ChatDialog::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==ui->searchWidget||obj==ui->sidebarWD||obj==ui->titleWD){
        if(event->type()==QEvent::MouseButtonPress){
            QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
            if(mouseEvent->button()==Qt::LeftButton){
                _isDragging=true;
                _dragPosition=mouseEvent->globalPosition().toPoint()-this->parentWidget()->pos();
                return true;
            }
        }
        if(event->type()==QEvent::MouseButtonRelease){
            QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
            if(mouseEvent->button()==Qt::LeftButton){
                _isDragging=false;
                return true;
            }
        }
        if(event->type()==QEvent::MouseMove){
            QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
            if(_isDragging){
                this->parentWidget()->move(mouseEvent->globalPos()-_dragPosition);
                return true;
            }
        }
    }
    return QDialog::eventFilter(obj,event);
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
    }
    else
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max.svg"));
        this->parentWidget()->showNormal();
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

void ChatDialog::uiInit()
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
