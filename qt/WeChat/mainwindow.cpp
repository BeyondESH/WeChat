#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>
#include "tcpclient.h"
#include "httpclient.h"
#include "usermanager.h"
#include "sessionmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(new QStackedWidget(this))
    , m_loginWidget(new LoginWidget(this))
    , m_contactListWidget(new ContactListWidget(this))
    , m_chatWindow(nullptr)
    , m_groupListWidget(new GroupListWidget(this))
    , m_currentUid(0)
{
    setupUi();
    setupConnections();

    setCentralWidget(m_stackedWidget);
    m_stackedWidget->addWidget(m_loginWidget);
    m_stackedWidget->addWidget(m_contactListWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    setWindowTitle("Distributed Chat System");
    resize(1200, 800);

    QMenuBar *menuBar = this->menuBar();
    QMenu *menuFile = menuBar->addMenu(tr("File"));

    QAction *actionLogout = new QAction(tr("Logout"), this);
    menuFile->addAction(actionLogout);
    connect(actionLogout, &QAction::triggered, this, &MainWindow::onLogout);

    QMenu *menuContacts = menuBar->addMenu(tr("Contacts"));

    QAction *actionSearchUser = new QAction(tr("Search User"), this);
    menuContacts->addAction(actionSearchUser);
    connect(actionSearchUser, &QAction::triggered, this, &MainWindow::onSearchUser);

    QMenu *menuGroup = menuBar->addMenu(tr("Group"));

    QAction *actionCreateGroup = new QAction(tr("Create Group"), this);
    menuGroup->addAction(actionCreateGroup);
    connect(actionCreateGroup, &QAction::triggered, this, &MainWindow::onCreateGroup);

    QAction *actionGroupList = new QAction(tr("Group List"), this);
    menuGroup->addAction(actionGroupList);
    connect(actionGroupList, &QAction::triggered, this, &MainWindow::loadGroupList);
}

void MainWindow::setupConnections()
{
    connect(m_loginWidget, &LoginWidget::loginSuccess,
            this, &MainWindow::onLoginSuccess);

    connect(m_contactListWidget, &ContactListWidget::openChat,
            this, &MainWindow::onOpenChat);

    connect(m_contactListWidget, &ContactListWidget::friendRequestReceived,
            this, &MainWindow::onFriendRequestReceived);
}

void MainWindow::onLoginSuccess(int uid, const QString &token, const QString &host, int port)
{
    m_currentUid = uid;
    m_currentToken = token;
    m_currentHost = host;
    m_currentPort = port;
    m_currentUserName = UserManager::instance()->getCurrentUserName();

    TcpClient::instance()->connectToServer(host, port);

    loadContactList();
    loadGroupList();

    if (m_chatWindow) {
        m_stackedWidget->removeWidget(m_chatWindow);
        delete m_chatWindow;
    }
    m_chatWindow = new ChatWindow(this);
    m_stackedWidget->addWidget(m_chatWindow);
    m_stackedWidget->addWidget(m_contactListWidget);

    m_stackedWidget->setCurrentWidget(m_contactListWidget);
}

void MainWindow::onLogout()
{
    TcpClient::instance()->disconnectFromServer();
    UserManager::instance()->clearCurrentUser();

    for (auto chatWindow : m_chatWindows) {
        delete chatWindow;
    }
    m_chatWindows.clear();

    for (auto chatWindow : m_groupChatWindows) {
        delete chatWindow;
    }
    m_groupChatWindows.clear();

    m_stackedWidget->setCurrentWidget(m_loginWidget);
    m_loginWidget->reset();
}

void MainWindow::loadContactList()
{
    HttpClient::instance()->getFriendList(m_currentUid, m_currentToken, [this](const QJsonArray &friends) {
        QVector<UserInfo> userList;
        for (const auto &friendJson : friends) {
            UserInfo info;
            info.uid = friendJson["uid"].toInt();
            info.name = friendJson["name"].toString();
            info.avatar = friendJson["avatar"].toString();
            info.status = friendJson["status"].toString();
            info.isOnline = friendJson["isOnline"].toBool();
            userList.append(info);
        }
        m_contactListWidget->updateContactList(userList);
    });
}

void MainWindow::loadGroupList()
{
    HttpClient::instance()->getGroupList(m_currentUid, m_currentToken, [this](const QJsonArray &groups) {
        QVector<GroupInfo> groupList;
        for (const auto &groupJson : groups) {
            GroupInfo info;
            info.groupId = groupJson["groupId"].toInt();
            info.name = groupJson["name"].toString();
            info.avatar = groupJson["avatar"].toString();
            info.memberCount = groupJson["memberCount"].toInt();
            groupList.append(info);
        }
        m_groupListWidget->updateGroupList(groupList);
    });
}

void MainWindow::onOpenChat(int uid, const QString &name)
{
    switchToChat(uid);
    if (m_chatWindows.contains(uid)) {
        m_chatWindows[uid]->setWindowTitle(name);
        m_stackedWidget->setCurrentWidget(m_chatWindows[uid]);
    }
}

void MainWindow::onOpenGroupChat(int groupId, const QString &groupName)
{
    switchToGroupChat(groupId);
    if (m_groupChatWindows.contains(groupId)) {
        m_groupChatWindows[groupId]->setWindowTitle(groupName);
        m_stackedWidget->setCurrentWidget(m_groupChatWindows[groupId]);
    }
}

void MainWindow::switchToChat(int uid)
{
    if (!m_chatWindows.contains(uid)) {
        ChatWindow *chatWindow = new ChatWindow(this);
        m_chatWindows[uid] = chatWindow;
        m_stackedWidget->addWidget(chatWindow);

        connect(chatWindow, &ChatWindow::sendTextMessage, this, [this, uid](const QString &content) {
            TcpClient::instance()->sendTextMessage(m_currentUid, uid, content);
        });

        connect(chatWindow, &ChatWindow::sendImageMessage, this, [this, uid](const QString &base64Data,
                                                                          const QString &imageType,
                                                                          int width, int height, int fileSize) {
            TcpClient::instance()->sendImageMessage(m_currentUid, uid, base64Data, imageType, width, height, fileSize);
        });

        HttpClient::instance()->getChatHistory(m_currentUid, uid, 50, 0, m_currentToken, [this, uid](const QJsonArray &messages) {
            QVector<MessageInfo> messageList;
            for (const auto &msgJson : messages) {
                MessageInfo msg;
                msg.messageId = msgJson["id"].toInt();
                msg.fromUid = msgJson["from_uid"].toInt();
                msg.toUid = msgJson["to_uid"].toInt();
                msg.content = msgJson["content"].toString();
                msg.sendTime = msgJson["send_time"].toString();
                msg.msgType = msgJson["msg_type"].toInt();
                msg.status = msgJson["status"].toInt();
                messageList.append(msg);
            }
            if (m_chatWindows.contains(uid)) {
                m_chatWindows[uid]->loadMessages(messageList);
            }
        });
    }
}

void MainWindow::switchToGroupChat(int groupId)
{
    if (!m_groupChatWindows.contains(groupId)) {
        ChatWindow *chatWindow = new ChatWindow(this);
        m_groupChatWindows[groupId] = chatWindow;
        m_stackedWidget->addWidget(chatWindow);

        connect(chatWindow, &ChatWindow::sendTextMessage, this, [this, groupId](const QString &content) {
            TcpClient::instance()->sendGroupTextMessage(m_currentUid, groupId, content);
        });

        connect(chatWindow, &ChatWindow::sendImageMessage, this, [this, groupId](const QString &base64Data,
                                                                                const QString &imageType,
                                                                                int width, int height, int fileSize) {
            TcpClient::instance()->sendGroupImageMessage(m_currentUid, groupId, base64Data, imageType, width, height, fileSize);
        });

        HttpClient::instance()->getGroupChatHistory(m_currentUid, groupId, 50, 0, m_currentToken, [this, groupId](const QJsonArray &messages) {
            QVector<MessageInfo> messageList;
            for (const auto &msgJson : messages) {
                MessageInfo msg;
                msg.messageId = msgJson["id"].toInt();
                msg.fromUid = msgJson["from_uid"].toInt();
                msg.groupId = msgJson["group_id"].toInt();
                msg.content = msgJson["content"].toString();
                msg.sendTime = msgJson["send_time"].toString();
                msg.msgType = msgJson["msg_type"].toInt();
                messageList.append(msg);
            }
            if (m_groupChatWindows.contains(groupId)) {
                m_groupChatWindows[groupId]->loadMessages(messageList);
            }
        });
    }
}

void MainWindow::onSearchUser()
{
    SearchUserDialog dialog(this);
    dialog.exec();
}

void MainWindow::onCreateGroup()
{
    bool ok;
    QString groupName = QInputDialog::getText(this, tr("Create Group"),
                                              tr("Group Name:"), QLineEdit::Normal, "", &ok);
    if (ok && !groupName.isEmpty()) {
        HttpClient::instance()->createGroup(m_currentUid, groupName, "", m_currentToken,
                                          [this](bool success, int groupId) {
            if (success) {
                QMessageBox::information(this, tr("Success"), tr("Group created successfully!"));
                loadGroupList();
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to create group!"));
            }
        });
    }
}

void MainWindow::onFriendRequestReceived(int fromUid, const QString &fromName, const QString &message)
{
    FriendRequestDialog dialog(fromUid, fromName, message, this);
    if (dialog.exec() == QDialog::Accepted) {
        bool accepted = dialog.isAccepted();
        TcpClient::instance()->sendAuthFriendResponse(m_currentUid, fromUid, accepted);
        loadContactList();
    }
}

void MainWindow::onNewMessageReceived(int fromUid, const QString &fromName, const QString &content, const QString &time)
{
    if (!m_chatWindows.contains(fromUid)) {
        QMessageBox::information(this, tr("New Message"),
                              QString("%1: %2").arg(fromName).arg(content));
    }

    if (m_chatWindows.contains(fromUid)) {
        MessageInfo msg;
        msg.fromUid = fromUid;
        msg.content = content;
        msg.sendTime = time;
        msg.msgType = 0;
        m_chatWindows[fromUid]->appendMessage(msg);
    }
}

void MainWindow::onNewImageMessageReceived(int fromUid, const QString &fromName,
                                          const QString &base64Image, const QString &imageType,
                                          int width, int height, const QString &time)
{
    if (!m_chatWindows.contains(fromUid)) {
        QMessageBox::information(this, tr("New Image Message"),
                              QString("%1 sent an image").arg(fromName));
    }

    if (m_chatWindows.contains(fromUid)) {
        MessageInfo msg;
        msg.fromUid = fromUid;
        msg.content = base64Image;
        msg.sendTime = time;
        msg.msgType = 1;
        msg.imageType = imageType;
        msg.imageWidth = width;
        msg.imageHeight = height;
        m_chatWindows[fromUid]->appendMessage(msg);
    }
}
