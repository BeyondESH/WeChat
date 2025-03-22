#include "messagewidget.h"
#include "ui_messagewidget.h"

MessageWidget::MessageWidget(QWidget *parent)
    : MessageBase(parent)
    , ui(new Ui::MessageWidget)
{
    ui->setupUi(this);
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::closeTime()
{
    ui->timeWD->close();
}

void MessageWidget::setMsgType(const MessageType &type)
{
    _msgType = type;
}

MessageType MessageWidget::getMsgType()
{
    return _msgType;
}

void MessageWidget::setMsgBody(const QString &body)
{
    _msgBody = body;
}

QString MessageWidget::getMsgBody()
{
    return _msgBody;
}

void MessageWidget::setMsgTime(const QDateTime &time)
{
    _msgTime = time;
}

QDateTime MessageWidget::getMsgTime()
{
    return _msgTime;
}

void MessageWidget::setMsgStatus(const MessageStatus &status)
{
    _msgStatus = status;
}

MessageStatus MessageWidget::getMsgStatus()
{
    return _msgStatus;
}

void MessageWidget::setInfo(const MessageType& msgType,const QString &headPath, const QDateTime &time, const QString &msg)
{
    _msgType = msgType;
    _headPath = headPath;
    _msgTime = time;
    _msgBody = msg;
    _msgStatus==MessageStatus::Sending;
    QLabel *bubbleLB=nullptr;
    if(msgType==MessageType::Receive){
        ui->sendWD->close();
        _contentWidget=ui->receiveWD;
        bubbleLB=ui->receiveBubbleLB;
    }else{
        ui->receiveWD->close();
        _contentWidget=ui->sendWD;
        bubbleLB=ui->sendBubbleLB;
    }

    // 设置头像
    QString styleSheet = QString("#%1{image: url(%2);}").arg(_contentWidget->objectName()).arg(headPath);
    _contentWidget->setStyleSheet(styleSheet);

    // 处理时间
    QDateTime now = QDateTime::currentDateTime();
    QDate today = now.date();
    QDate messageDate = _msgTime.date();
    if(messageDate==today){
        ui->timeLB->setText(_msgTime.toString("h:mm"));
    }else{
        ui->timeLB->setText(_msgTime.toString("yy/M/d h:mm"));
    }

    // 设置消息
    bubbleLB->setText(msg);

}

void MessageWidget::setInfo(const MessageType &msgType, const QString &headPath, const QString &msg)
{
    _msgType = msgType;
    _headPath = headPath;
    _msgBody = msg;
    _msgStatus==MessageStatus::Sending;
    QLabel *bubbleLB=nullptr;
    if(msgType==MessageType::Receive){
        ui->sendWD->close();
        _contentWidget=ui->receiveWD;
        bubbleLB=ui->receiveBubbleLB;
    }else{
        ui->receiveWD->close();
        _contentWidget=ui->sendWD;
        bubbleLB=ui->sendBubbleLB;
    }

    // 设置头像
    QString styleSheet = QString("#%1{image: url(%2);}").arg(_contentWidget->objectName()).arg(headPath);
    _contentWidget->setStyleSheet(styleSheet);


    // 设置消息
    bubbleLB->setText(msg);
}
