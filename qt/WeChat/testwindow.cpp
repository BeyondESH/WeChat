#include "testwindow.h"
#include "logindialog.h"//登录页面
#include "registerdialog.h"//注册页面
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
