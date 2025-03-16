#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void addChatUserList();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private slots:
    void on_searchLE_textEdited(const QString &arg1);
    void on_clearPB_clicked();
    void on_topPB_toggled(bool checked);
    void on_maxPB_toggled(bool checked);
    void on_searchLE_textChanged(const QString &arg1);
    void on_chatCB_clicked();
    void on_usersCB_clicked();
    void on_quitPB_clicked();
    void on_minPB_clicked();
    void slots_loading_chat_user();
private:
    Ui::ChatDialog *ui;
    void uiInit();
    bool _isDragging = false;
    QPoint _dragPosition;
    bool _isLoading = false;

signals:
    void signal_loading_chat_user();
};

#endif // CHATDIALOG_H
