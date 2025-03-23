#include "messagewidget.h"
#include "ui_messagewidget.h"
#include <QTimer>
#include <QPainter>
#include <QTextBlock>
#include <QFontMetricsF>
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
    // 设置文本框大小
    qreal doc_margin = textEdit->document()->documentMargin();
    int margin_left = textEdit->contentsMargins().left();
    int margin_right = textEdit->contentsMargins().right();
    qDebug()<<doc_margin<<margin_left<<margin_right;
    QFontMetricsF fm(textEdit->font());
    QTextDocument *doc = textEdit->document();
    int max_width = 0;
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
    {
        int txtW = int(fm.horizontalAdvance(it.text()));
        max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
    }
    textEdit->setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right));
    // 设置高度
    textEdit->document()->adjustSize();
    textEdit->document()->setTextWidth(textEdit->viewport()->width());
    qreal text_height = textEdit->document()->size().height();
    qDebug()<<"text_height:"<<text_height;
    textEdit->setFixedHeight(text_height + 20);
    qDebug()<<"textEdit height:"<<textEdit->height();
}
