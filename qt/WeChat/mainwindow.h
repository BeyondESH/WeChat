#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      主窗口
 *
 * @author     李佳承
 * @date       2024/10/30
 * @history
 *****************************************************************************/

#include <QMainWindow>
#include "logindialog.h"//登录页面
#include "registerdialog.h"//注册页面
#include "resetdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginDialog *_loginDialog;
    RegisterDialog *_registerDialog;
    ResetDialog *_resetDialog;

public slots:
    void openLoginDialog();
    void openRegisterDialog();
    void openResetDialog();
};
#endif // MAINWINDOW_H
