#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H
/******************************************************************************
 *
 * @file       registerdialog.h
 * @brief      注册页面
 *
 * @author     李佳承
 * @date       2024/10/31
 * @history
 *****************************************************************************/
#include <QDialog>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_backPushButton_clicked();
    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_checkBox_2_checkStateChanged(const Qt::CheckState &arg1);
    void on_getCodepushButton_clicked();
    void slot_mod_register_finished(ReqId req_id,QString res,ErrorCodes ec);
    void on_submitPushButton_clicked();
    void on_accountLineEdit_textEdited(const QString &arg1);
    void on_accountLineEdit_editingFinished();
    void on_emailLineEdit_editingFinished();
    void on_emailLineEdit_textEdited(const QString &arg1);
    void on_passwordLineEdit_textEdited(const QString &arg1);
    void on_verifylineEdit_textEdited(const QString &arg1);
    void on_verifylineEdit_editingFinished();
    void on_passwordLineEdit_editingFinished();
    void on_codeLineEdit_editingFinished();
signals:
    void backPBClicked();
private:
    void polish();//刷新qss
    void showTip(QString tip,bool isOK);//显示提示
    Ui::RegisterDialog *ui;
    void initHttpHandlers();//初始化http请求处理函数
    QMap<ReqId,std::function<void(const QJsonObject& jsonObj)>> _handlers;
    int remainTime;//验证码倒计时
    std::vector<int> _corrects;
};

#endif // REGISTERDIALOG_H
