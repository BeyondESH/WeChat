#ifndef TESTWINDOW_H
#define TESTWINDOW_H
/******************************************************************************
 *
 * @file       testwindow.h
 * @brief      测试窗口的抽象类
 *
 * @author     李佳承
 * @date       2024/10/31
 * @history
 *****************************************************************************/
#include "mainwindow.h"

class testWindow
{
public:
    testWindow()=delete;

    //测试登录页面
    static void logindialog(MainWindow &w);
    //测试注册页面
    static void registerdialog(MainWindow &w);
    //测试聊天页面
    static void chatdialog(MainWindow &w);
    //测试用户列表
    static void chatuserlist(MainWindow &w);

    static void messageWidget(MainWindow &w);

    static std::vector<QString> generateMsg();

    static std::vector<QString> generateHeads();

    static std::vector<QString> generateName();

    static QDate generateRandomDate(const QDate& minDate, const QDate& maxDate);

    static QTime generateRandomTime();

    static QDateTime generateRandomDateTime(const QDate& minDate, const QDate& maxDate);
};

#endif // TESTWINDOW_H
