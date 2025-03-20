#include "testwindow.h"
#include "logindialog.h"//登录页面
#include "registerdialog.h"//注册页面
#include "chatdialog.h"//聊天界面
#include "messagewidget.h"
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
