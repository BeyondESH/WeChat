#include "chatpagewidget.h"
#include "ui_chatpagewidget.h"
#include <QEvent>
#include <QMouseEvent>
#include <QDate>
#include <QDateTime>
#include <QRandomGenerator>
#include "messagewidget.h"
#include "testwindow.h"
#include "usermgr.h"
#include <QTimer>
ChatPageWidget::ChatPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatPageWidget)
{
    ui->setupUi(this);
    ui->chatTE->installEventFilter(this);
}

ChatPageWidget::~ChatPageWidget()
{
    delete ui;
}

QWidget *ChatPageWidget::getTitleWidget() const
{
    return ui->titleWD;
}

void ChatPageWidget::on_quitPB_clicked()
{
    QApplication::quit();
}

void ChatPageWidget::on_minPB_clicked()
{
    this->parentWidget()->parentWidget()->parentWidget()->showMinimized();
}

void ChatPageWidget::on_maxPB_toggled(bool checked)
{
    auto pb = ui->maxPB;
    if (checked)
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max_clicked.svg"));
        this->parentWidget()->parentWidget()->parentWidget()->showMaximized();
    }
    else
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max.svg"));
        this->parentWidget()->parentWidget()->parentWidget()->showNormal();
    }
}


void ChatPageWidget::on_topPB_toggled(bool checked)
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

void ChatPageWidget::on_sendPB_clicked()
{
    auto chatTE = ui->chatTE;
    if(chatTE->toPlainText().isEmpty()){
        qDebug()<<"发送内容不能为空";
        return;
    }
    MessageType type=MessageType::Send;
    QString text=chatTE->toPlainText();
    const QString userIcon=UserMgr::getInstance()->getHeadPath();
}

bool ChatPageWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==ui->chatTE){
        if(event->type()==QEvent::KeyPress){
            QKeyEvent* keyEvent=static_cast<QKeyEvent*>(event);
            if(keyEvent->key()==Qt::Key_Return ||keyEvent->key()==Qt::Key_Enter){
                on_sendPB_clicked();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj,event);
}

