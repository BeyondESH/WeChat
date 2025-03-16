#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QScrollBar>
#include <QMouseEvent>
#include "chatuserwidget.h"
#include <QRandomGenerator>
ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    uiInit();
    ui->searchWidget->installEventFilter(this);
    ui->sidebarWD->installEventFilter(this);
    ui->titleWD->installEventFilter(this);
    ui->chatUserList->installEventFilter(this);
    addChatUserList();
    ui->chatUserList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

std::vector<QString>  strs ={"你好",
                             "This is the way",
                             "I love you",
                             "I miss you",
                             "My love is written in the wind ever since the whole world is you",
                             "这些数据只是测试数据"};
std::vector<QString> heads = {
    ":/img/loginDialog/img/avatar/AL.jpg",
    ":/img/loginDialog/img/avatar/BS.jpg",
    ":/img/loginDialog/img/avatar/DB.jpg",
    ":/img/loginDialog/img/avatar/JZ.jpg",
    ":/img/loginDialog/img/avatar/LM.jpg",
    ":/img/loginDialog/img/avatar/MAO.jpg",
    ":/img/loginDialog/img/avatar/RK.jpg",
    ":/img/loginDialog/img/avatar/SY.jpg",
    ":/img/loginDialog/img/avatar/wz.jpg",
    ":/img/loginDialog/img/avatar/YZ.jpg"
};
std::vector<QString> names = {
    "阿柳",
    "Boyer Shaw",
    "阿杜",
    "JZ",
    "老梅",
    "毛",
    "软壳",
    "盛誉",
    "吴征",
    "余卓狗"
};

// 生成一个随机的 QDate
QDate generateRandomDate(const QDate& minDate, const QDate& maxDate) {
    int minJulianDay = minDate.toJulianDay();
    int maxJulianDay = maxDate.toJulianDay();
    int randomJulianDay = QRandomGenerator::global()->bounded(minJulianDay, maxJulianDay + 1);
    return QDate::fromJulianDay(randomJulianDay);
}

// 生成一个随机的 QTime
QTime generateRandomTime() {
    int hour = QRandomGenerator::global()->bounded(0, 24);     // 0到23小时
    int minute = QRandomGenerator::global()->bounded(0, 60);    // 0到59分钟
    int second = QRandomGenerator::global()->bounded(0, 60);    // 0到59秒
    int msec = QRandomGenerator::global()->bounded(0, 1000);    // 0到999毫秒
    return QTime(hour, minute, second, msec);
}

// 生成一个随机的 QDateTime
QDateTime generateRandomDateTime(const QDate& minDate, const QDate& maxDate) {
    QDate randomDate = generateRandomDate(minDate, maxDate);
    QTime randomTime = generateRandomTime();
    return QDateTime(randomDate, randomTime);
}

void ChatDialog::addChatUserList()
{
    for (int i = 0; i < 20; i++)
    {
        QDate minDate(2025, 3, 13);  // 最小日期
        QDate maxDate=QDate::currentDate();  // 最大日期
        int rand = QRandomGenerator::global()->bounded(0, 100);
        int str_i = rand%strs.size();
        int head_i = rand%heads.size();
        int name_i = rand%names.size();
        ChatUserWidget* itemWidget=new ChatUserWidget(ui->chatUserList);
        itemWidget->setInfo(names[name_i],heads[head_i],generateRandomDateTime(minDate,maxDate),strs[str_i]);
        QListWidgetItem* item=new QListWidgetItem(ui->chatUserList);
        item->setSizeHint(itemWidget->sizeHint());
        ui->chatUserList->addItem(item);
        ui->chatUserList->setItemWidget(item,itemWidget);
    }
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

    if(obj==ui->chatUserList){
        if(event->type()==QEvent::Enter){
            // 启用像素级滚动
            ui->chatUserList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            // 调整滚动速度
            ui->chatUserList->verticalScrollBar()->setSingleStep(10);
            ui->chatUserList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            return true;
        }else if(event->type()==QEvent::Leave){
            ui->chatUserList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            return true;
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
