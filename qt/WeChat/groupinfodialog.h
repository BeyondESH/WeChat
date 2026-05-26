#ifndef GROUPINFODIALOG_H
#define GROUPINFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class GroupInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupInfoDialog(int groupId, QWidget *parent = nullptr);
    ~GroupInfoDialog();

private slots:
    void onAddMemberClicked();
    void onRemoveMemberClicked();
    void loadGroupMembers();

private:
    void setupUi();

    int m_groupId;
    QListWidget *m_memberList;
    QPushButton *m_addMemberBtn;
    QPushButton *m_removeMemberBtn;
    QPushButton *m_closeBtn;
};

#endif // GROUPINFODIALOG_H
