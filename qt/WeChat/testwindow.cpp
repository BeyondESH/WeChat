#include "testwindow.h"
#include "logindialog.h"//登录页面
#include "registerdialog.h"//注册页面
#include "chatdialog.h"//聊天界面
#include "messagewidget.h"
#include <QRandomGenerator>
void testWindow::logindialog(MainWindow &w)
{
    w.hide();
    LoginDialog *l=new LoginDialog();
    l->show();
}

void testWindow::registerdialog(MainWindow &w)
{
    w.hide();
    RegisterDialog *r=new RegisterDialog();
    r->show();
}

void testWindow::chatdialog(MainWindow &w)
{
    w.hide();
    ChatDialog *c=new ChatDialog();
    c->show();
}

void testWindow::messageWidget(MainWindow &w)
{
    w.hide();
    MessageWidget *m=new MessageWidget();
    m->show();
}

std::vector<QString> testWindow::generateMsg()
{
    std::vector<QString>  strs ={"你好",
                                 "This is the way",
                                 "I love you",
                                 "My love is written in the wind ever since the whole world is you,在制作聊天框时，我打算通过QListWidget来实现,作为对话框气泡的显示区域，我将messageWidget作为QListWidget（msgList）的一个ListItem，来实现显示聊天时间和人物头像以及气泡对话框的功能，我的思路是，messageBase先判断这条聊天消息是接收的还是发送的，如果是发送的，就只显示messageWidget中右边的头像和气泡对话框，也就是rightWD，反之亦然，对于时间timeWD，如果消息的时间间隔超过1分钟就显示timeWD，小于1分钟就hide()，以及我还面临一个问题，就是气泡对话框和头像之间我像做一个三角指向头像中间，这个三角不管气泡对话框的高度变化，始终位置指向头像中间，仿造微信的气泡对话框的样式，总之我这个项目就是用qt复刻微信，一起样式都要仿造微信的样式。请你帮我完成",
                                 "这些数据只是测试数据"};
    return strs;
}

std::vector<QString> testWindow::generateHeads()
{
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
    return heads;
}

std::vector<QString> testWindow::generateName()
{
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
    return names;
}

// 生成一个随机的 QDate
QDate testWindow::generateRandomDate(const QDate& minDate, const QDate& maxDate) {
    int minJulianDay = minDate.toJulianDay();
    int maxJulianDay = maxDate.toJulianDay();
    int randomJulianDay = QRandomGenerator::global()->bounded(minJulianDay, maxJulianDay + 1);
    return QDate::fromJulianDay(randomJulianDay);
}

// 生成一个随机的 QTime
QTime testWindow::generateRandomTime() {
    int hour = QRandomGenerator::global()->bounded(0, 24);     // 0到23小时
    int minute = QRandomGenerator::global()->bounded(0, 60);    // 0到59分钟
    int second = QRandomGenerator::global()->bounded(0, 60);    // 0到59秒
    int msec = QRandomGenerator::global()->bounded(0, 1000);    // 0到999毫秒
    return QTime(hour, minute, second, msec);
}

// 生成一个随机的 QDateTime
QDateTime testWindow::generateRandomDateTime(const QDate& minDate, const QDate& maxDate) {
    QDate randomDate = generateRandomDate(minDate, maxDate);
    QTime randomTime = generateRandomTime();
    return QDateTime(randomDate, randomTime);
}
