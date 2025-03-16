#include "chatuserwidget.h"
#include "ui_chatuserwidget.h"

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWidget)
{
    ui->setupUi(this);
    setItemTye(CHAT_USER_ITEM);
    ui->distrubPB->close();
}

ChatUserWidget::~ChatUserWidget()
{
    delete ui;
}

QSize ChatUserWidget::sizeHint() const
{
    return QSize(225,64);
}

void ChatUserWidget::setInfo(const QString &name, const QString &headPath, const QDateTime &time, const QString &msg)
{
    _name=name;
    _headPath=headPath;
    _time=time;
    _msg=msg;

    //设置头像
    QString styleSheet=QString("#avatarLB{image: url(%1);}").arg(headPath);
    ui->avatarLB->setStyleSheet(styleSheet);
    //设置名字
    ui->userNameLB->setText(_name);
    //设置消息
    ui->userNameLB->setMaximumWidth(100);
    ui->msgLB->setMaximumWidth(100);
    ui->msgLB->setText(_msg);
    //处理时间
    QDate today = QDate::currentDate();
    QDate messageDate = _time.date();
    
    if (messageDate == today) {
        // 今天发送的消息，只显示时间
        ui->timeLB->setText(_time.toString("hh:mm"));
    } else if (messageDate == today.addDays(-1)) {
        // 昨天发送的消息
        ui->timeLB->setText("昨天");
    } else if (messageDate == today.addDays(-2)) {
        // 前天发送的消息
        ui->timeLB->setText("前天");
    } else {
        // 更早的消息，显示日期
        ui->timeLB->setText(_time.toString("yy/M/d"));
    }
}


