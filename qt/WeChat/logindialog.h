#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
/******************************************************************************
 *
 * @file       logindialog.h
 * @brief      登录页面
 *
 * @author     李佳承
 * @date       2024/10/30
 * @history
 *****************************************************************************/
#include <QDialog>
#include "global.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    //刷新qss
    void polish();

private slots:
    void on_registerPushButton_clicked();
    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_loginButton_clicked();
    void on_resetPasswordPB_clicked();
    void slot_mod_login_finished(ReqId req_id,QString res,ErrorCodes ec);
    void slot_tcp_connect_finished(bool isSuccess);
    void slot_login_failed(int error);
    void on_quitPB_clicked();

signals:
    void registerPBClicked();//注册按钮
    void resetPasswordPBClicked();//忘记密码
    void signal_tcp_connected(ServerInfo info);//tcp连接
private:
    Ui::LoginDialog *ui;
    void showTip(QString tip,bool isOK);//显示提示
    void initHttpHandlers();//初始化http请求处理函数
    QMap<ReqId,std::function<void(const QJsonObject& jsonObj)>> _handlers;
    int _uid;
    QString _token;
};

#endif // LOGINDIALOG_H
