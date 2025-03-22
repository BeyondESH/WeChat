#include "messagewidget.h"
#include "ui_messagewidget.h"
#include <QTimer>
MessageWidget::MessageWidget(QWidget *parent)
    : MessageBase(parent)
    , ui(new Ui::MessageWidget)
{
    ui->setupUi(this);
    ui->sendTE->setReadOnly(true);
    ui->receviceTE->setReadOnly(true);
    ui->sendTE->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->receviceTE->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->sendTE->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->receviceTE->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->sendTE->setFrameShape(QFrame::NoFrame);
    ui->receviceTE->setFrameShape(QFrame::NoFrame);
    ui->sendTE->setFocusPolicy(Qt::NoFocus);
    ui->receviceTE->setFocusPolicy(Qt::NoFocus);
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::closeTime()
{
    ui->timeWD->close();
}

QSize MessageWidget::sizeHint() const
{
    return QSize(width(), height());
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
    _msgStatus=MessageStatus::Sending;
    QTextEdit *textEdit=nullptr;
    QLabel *headLB=nullptr;
    if(msgType==MessageType::Receive){
        ui->sendHeadLB->close();
        ui->sendTE->close();
        textEdit=ui->receviceTE;
        headLB=ui->receiveHeadLB;
    }else{
        ui->receiveHeadLB->close();
        ui->receviceTE->close();
        textEdit=ui->sendTE;
        headLB=ui->sendHeadLB;
    }

    // 设置头像
    QString styleSheet = QString("#%1{image: url(%2);}").arg(headLB->objectName()).arg(headPath);
    headLB->setStyleSheet(styleSheet);

    // 处理时间
    QDateTime now = QDateTime::currentDateTime();
    QDate today = now.date();
    QDate messageDate = _msgTime.date();
    if(messageDate==today){
        ui->timeLB->setText(_msgTime.toString("h:mm"));
    }else{
        ui->timeLB->setText(_msgTime.toString("yy年M月d日 h:mm"));
    }
    // 设置消息
    textEdit->setText(msg);
    int parentWidth = this->width();//父容器宽度
    int maxTextWidth = parentWidth * 0.7; // 最大宽度为窗口的70%
    int minTextWidth = 60; // 最小宽度
    int horizontalPadding = 20; 
    // 使用QFontMetrics计算文本宽度
    QFontMetrics fm(textEdit->font());
    QStringList lines = msg.split('\n');
    int textWidth = 0;

    // 查找最长的一行
    for (const QString& line : lines) {
        int lineWidth = fm.horizontalAdvance(line);
        textWidth = qMax(textWidth, lineWidth);
    }

    textWidth += horizontalPadding;
    textWidth = qMax(minTextWidth, qMin(textWidth, maxTextWidth));
    // 设置文档宽度以便自动换行
    textEdit->document()->setTextWidth(textWidth-horizontalPadding);
    // 计算适当的高度
    int contentHeight = textEdit->document()->size().height();
    int verticalPadding = 24;
    int minHeight = 36;
    textEdit->setMinimumWidth(textWidth);
    textEdit->setMaximumWidth(textWidth);
    int calculatedHeight = contentHeight + verticalPadding;
    textEdit->setMinimumHeight(qMax(minHeight, calculatedHeight));
    // 移除最大高度限制，允许文本框根据内容扩展
    textEdit->setMaximumHeight(QWIDGETSIZE_MAX);
    adjustSize();
}
