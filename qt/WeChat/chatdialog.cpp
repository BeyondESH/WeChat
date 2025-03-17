#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QScrollBar>
#include <QMouseEvent>
#include "chatuserwidget.h"
#include <QRandomGenerator>
#include "chatpagewidget.h"
ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog),_isDragging(false),_isLoading(false)
{
    ui->setupUi(this);
    uiInit();
    connect(this,&ChatDialog::signal_loading_chat_user,this,&ChatDialog::slots_loading_chat_user);
    ui->searchWidget->installEventFilter(this);
    ui->sidebarWD->installEventFilter(this);
    ui->chatPageSW->installEventFilter(this);
    ui->chatUserList->installEventFilter(this);
    static_cast<ChatPageWidget *>(ui->chatPageSW->currentWidget())->getTitleWidget()->installEventFilter(this);
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
    for (int i = 0; i < 100; i++)
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
    //窗口拖动
    ChatPageWidget *chatPageWidget=static_cast<ChatPageWidget *>(ui->chatPageSW->currentWidget());
    if(obj==ui->searchWidget||obj==ui->sidebarWD||obj==chatPageWidget->getTitleWidget()){
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
                this->parentWidget()->move(mouseEvent->globalPosition().toPoint()-_dragPosition);
                return true;
            }
        }
    }

    //滚动体设置
    if(obj==ui->chatUserList){
        if(event->type()==QEvent::Enter){
            // 启用像素级滚动
            ui->chatUserList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            // 调整滚动速度
            ui->chatUserList->verticalScrollBar()->setSingleStep(8);
            ui->chatUserList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            QString styleSheet = ui->chatUserList->styleSheet();
            return false;
        }else if(event->type()==QEvent::Leave){
            ui->chatUserList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            QString styleSheet = ui->chatUserList->styleSheet();
            return false;
        }else if(event->type()==QEvent::Wheel){
            QScrollBar* scrollBar=ui->chatUserList->verticalScrollBar();
            int maxScrollValue=scrollBar->maximum();
            int currentValue=scrollBar->value();
            if(maxScrollValue-currentValue<=0){
                qDebug()<<"加载更多内容";
                emit signal_loading_chat_user();
            }
            return false;
        }
    }

    //双击缩放
    if(obj==ui->searchWidget||obj==chatPageWidget->getTitleWidget()){
        if(event->type()==QEvent::MouseButtonDblClick){
            if(this->parentWidget()->isMaximized()){
                this->parentWidget()->showNormal();
            }else{
                this->parentWidget()->showMaximized();
            }
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

void ChatDialog::slots_loading_chat_user()
{
    //上一轮加载未完成
    if(_isLoading){
        return;
    }

    addChatUserList();
    _isLoading=true;
}
