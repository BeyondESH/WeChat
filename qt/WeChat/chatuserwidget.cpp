#include "chatuserwidget.h"
#include "ui_chatuserwidget.h"

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWidget)
{
    ui->setupUi(this);
    setItemTye(CHAT_USER_ITEM);
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
    ui->msgLB->setText(_msg);

    //处理时间
    QDateTime today=QDateTime::currentDateTime();//获取当前日期
    int days=today.daysTo(_time);//获取两个日期之间的天数
    if(days==2){
        ui->timeLB->setText("前天");
    }else if(days==1){
        ui->timeLB->setText("昨天");
    }else if(days==0){
        ui->timeLB->setText(_time.toString("hh:mm"));
    }else{
        ui->timeLB->setText(_time.toString("yyyy-MM-dd"));
    }
}


