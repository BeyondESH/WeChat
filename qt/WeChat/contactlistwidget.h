#ifndef CONTACTLISTWIDGET_H
#define CONTACTLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "usermanager.h"

class ContactListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactListWidget(QWidget *parent = nullptr);
    ~ContactListWidget();
    void updateContactList(const QVector<UserInfo> &contacts);

signals:
    void openChat(int uid, const QString &name);
    void friendRequestReceived(int fromUid, const QString &fromName, const QString &message);

private slots:
    void onContactClicked(QListWidgetItem *item);
    void onAddFriendClicked();
    void onRefreshClicked();

private:
    void setupUi();

    QListWidget *m_contactList;
    QPushButton *m_addFriendBtn;
    QPushButton *m_refreshBtn;
    QMap<int, UserInfo> m_contacts;
};

#endif // CONTACTLISTWIDGET_H
