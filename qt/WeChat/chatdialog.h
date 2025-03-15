#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

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
private:
    Ui::ChatDialog *ui;
    void init();

};

#endif // CHATDIALOG_H
