#include "chatuserwidget.h"
#include "ui_chatuserwidget.h"

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : ListItemBase(parent), ui(new Ui::ChatUserWidget)
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
    return QSize(225, 64);
}

void ChatUserWidget::setInfo(const QString &name, const QString &headPath, const QDateTime &time, const QString &msg)
{
    _name = name;
    _headPath = headPath;
    _time = time;
    _msg = msg;

    // 设置头像
    QString styleSheet = QString("#avatarLB{image: url(%1);}").arg(headPath);
    ui->avatarLB->setStyleSheet(styleSheet);

    // 设置名字 - 使用 elidedText 处理溢出
    QFontMetrics nameMetrics(ui->userNameLB->font());
    QString elidedName = nameMetrics.elidedText(_name, Qt::ElideRight, 120);
    ui->userNameLB->setText(elidedName);
    ui->userNameLB->setMaximumWidth(120);
    ui->userNameLB->setToolTip(_name); // 添加工具提示显示完整文本

    // 设置消息 - 使用 elidedText 处理溢出
    QFontMetrics msgMetrics(ui->msgLB->font());
    QString elidedMsg = msgMetrics.elidedText(_msg, Qt::ElideRight, 120);
    ui->msgLB->setText(elidedMsg);
    ui->msgLB->setMaximumWidth(120);
    ui->msgLB->setToolTip(_msg); // 添加工具提示显示完整文本

    // 处理时间
    QDateTime now = QDateTime::currentDateTime();
    QDate today = now.date();
    QDate messageDate = _time.date();

    // 计算时间差（以分钟为单位）
    qint64 minutesDiff = _time.secsTo(now) / 60;

    // 只有消息时间是今天且在过去的2分钟内才显示"刚刚"
    if (minutesDiff >= 0 && minutesDiff <= 2 && messageDate == today)
    {
        // 2分钟内的消息，显示"刚刚"
        ui->timeLB->setText("刚刚");
    }
    else if (messageDate == today)
    {
        // 今天发送的消息，只显示时间
        ui->timeLB->setText(_time.toString("h:mm"));
    }
    else if (messageDate == today.addDays(-1))
    {
        // 昨天发送的消息
        ui->timeLB->setText("昨天");
    }
    else if (messageDate == today.addDays(-2))
    {
        // 前天发送的消息
        ui->timeLB->setText("前天");
    }
    else
    {
        // 更早的消息，显示日期
        ui->timeLB->setText(_time.toString("yy/M/d"));
    }
}
