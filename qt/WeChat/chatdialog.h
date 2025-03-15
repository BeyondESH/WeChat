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

private slots:
    void on_searchLE_textEdited(const QString &arg1);

    void on_clearPB_clicked();
    void on_topPB_toggled(bool checked);

    void on_maxPB_toggled(bool checked);

private:
    Ui::ChatDialog *ui;
};

#endif // CHATDIALOG_H
