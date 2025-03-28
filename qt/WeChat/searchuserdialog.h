#ifndef SEARCHUSERDIALOG_H
#define SEARCHUSERDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "global.h"

namespace Ui {
class SearchUserDialog;
}

class SearchUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchUserDialog(QWidget *parent = nullptr);
    ~SearchUserDialog();
    
    // 显示对话框
    void showDialog();

private slots:
    void on_searchPB_clicked();
    void on_cancelPB_clicked();
    void onSearchSuccess(const QVector<UserInfo>& users);
    void onSearchFailed(int error);
    void onUserItemClicked(QListWidgetItem* item);
    void onAddFriendSuccess();
    void onAddFriendFailed(int error);

private:
    Ui::SearchUserDialog *ui;
    void clearResults();
    QVector<UserInfo> _searchResults;

signals:
    void friendSelected(const UserInfo& userInfo);
};

#endif // SEARCHUSERDIALOG_H
