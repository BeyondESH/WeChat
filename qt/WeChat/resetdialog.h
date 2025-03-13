#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"
namespace Ui
{
    class resetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

protected:
    // 添加鼠标事件处理函数
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_backPushButton_clicked();
    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_checkBox_2_checkStateChanged(const Qt::CheckState &arg1);
    void on_getCodepushButton_clicked();
    void slot_mod_register_finished(ReqId req_id, QString res, ErrorCodes ec);
    void on_submitPushButton_clicked();
    void on_emailLineEdit_editingFinished();
    void on_emailLineEdit_textEdited(const QString &arg1);
    void on_passwordLineEdit_textEdited(const QString &arg1);
    void on_verifylineEdit_textEdited(const QString &arg1);
    void on_verifylineEdit_editingFinished();
    void on_passwordLineEdit_editingFinished();
    void on_codeLineEdit_editingFinished();

    void on_quitPB_clicked();

signals:
    void backPBClicked();

private:
    Ui::resetDialog *ui;
    void polish();                        // 刷新qss
    void showTip(QString tip, bool isOK); // 显示提示
    void initHttpHandlers();              // 初始化http请求处理函数
    QMap<ReqId, std::function<void(const QJsonObject &jsonObj)>> _handlers;
    int remainTime; // 验证码倒计时
    std::vector<int> _corrects;
    QPoint dragPosition;     // 添加记录拖动位置的变量
    bool isDragging = false; // 添加标记是否正在拖动的变量
};

#endif // RESETDIALOG_H
