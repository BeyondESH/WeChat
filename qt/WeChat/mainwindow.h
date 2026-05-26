#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMap>
#include "loginwidget.h"
#include "contactlistwidget.h"
#include "chatwindow.h"
#include "grouplistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginSuccess(int uid, const QString &token, const QString &host, int port);
    void onLogout();
    void onOpenChat(int uid, const QString &name);
    void onOpenGroupChat(int groupId, const QString &groupName);
    void onSearchUser();
    void onCreateGroup();
    void onFriendRequestReceived(int fromUid, const QString &fromName, const QString &message);
    void onNewMessageReceived(int fromUid, const QString &fromName, const QString &content, const QString &time);
    void onNewImageMessageReceived(int fromUid, const QString &fromName, const QString &base64Image,
                                   const QString &imageType, int width, int height, const QString &time);

private:
    void setupUi();
    void setupConnections();
    void loadContactList();
    void loadGroupList();
    void switchToChat(int uid);
    void switchToGroupChat(int groupId);

    QStackedWidget *m_stackedWidget;
    LoginWidget *m_loginWidget;
    ContactListWidget *m_contactListWidget;
    ChatWindow *m_chatWindow;
    GroupListWidget *m_groupListWidget;
    QMap<int, ChatWindow*> m_chatWindows;
    QMap<int, ChatWindow*> m_groupChatWindows;

    int m_currentUid;
    QString m_currentToken;
    QString m_currentHost;
    int m_currentPort;
    QString m_currentUserName;
};

#endif // MAINWINDOW_H
