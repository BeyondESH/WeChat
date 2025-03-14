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
    void on_searchLE_editingFinished();

    void on_searchLE_textEdited(const QString &arg1);

private:
    Ui::ChatDialog *ui;
};

#endif // CHATDIALOG_H
