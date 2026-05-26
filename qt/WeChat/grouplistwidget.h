#ifndef GROUPLISTWIDGET_H
#define GROUPLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "usermanager.h"

class GroupListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupListWidget(QWidget *parent = nullptr);
    ~GroupListWidget();
    void updateGroupList(const QVector<GroupInfo> &groups);

signals:
    void openGroupChat(int groupId, const QString &groupName);
    void groupInfoRequested(int groupId);

private slots:
    void onGroupClicked(QListWidgetItem *item);
    void onGroupInfoClicked();
    void onRefreshClicked();

private:
    void setupUi();

    QListWidget *m_groupList;
    QPushButton *m_groupInfoBtn;
    QPushButton *m_refreshBtn;
    QMap<int, GroupInfo> m_groups;
};

#endif // GROUPLISTWIDGET_H
