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

ChatPageWidget::ChatPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatPageWidget)
{
    ui->setupUi(this);
    ui->msgList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->msgList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    addListItem();
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

void ChatPageWidget::addListItem()
{
    std::vector<QString>strs=testWindow::generateMsg();
    std::vector<QString>heads=testWindow::generateHeads();
    std::vector<QString>names=testWindow::generateName();
    for (int i = 0; i < 10; i++)
    {
        QDate minDate(2025, 3, 13);  // 最小日期
        QDate maxDate=QDate::currentDate();  // 最大日期
        int rand = QRandomGenerator::global()->bounded(0, 100);
        int str_i = rand%strs.size();
        int head_i = rand%heads.size();
        MessageWidget* itemWidget=new MessageWidget(ui->msgList);
        itemWidget->setInfo(MessageType::Receive,heads[head_i],testWindow::generateRandomDateTime(minDate,maxDate),strs[str_i]);
        QListWidgetItem* item=new QListWidgetItem(ui->msgList);
        item->setSizeHint(itemWidget->sizeHint());
        ui->msgList->addItem(item);
        ui->msgList->setItemWidget(item,itemWidget);
    }
}

void ChatPageWidget::on_sendPB_clicked()
{
    if(ui->chatTE->toPlainText().isEmpty()){
        qDebug()<<"发送内容不能为空";
        return;
    }
    QString msg=ui->chatTE->toPlainText();
    QDateTime time=QDateTime::currentDateTime();
    MessageWidget * messageWidget=new MessageWidget(ui->msgList);
    messageWidget->setInfo(MessageType::Send,UserMgr::getInstance()->getHeadPath(),time,msg);
    QListWidgetItem *item=new QListWidgetItem(ui->msgList);
    item->setSizeHint(messageWidget->sizeHint());
    ui->msgList->addItem(item);
    ui->msgList->setItemWidget(item,messageWidget);
    ui->chatTE->clear();
}

