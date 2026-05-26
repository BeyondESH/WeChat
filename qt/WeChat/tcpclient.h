#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "singleton.h"

class TcpClient : public QObject, public Singleton<TcpClient>
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();

    void sendLoginRequest(int uid, const QString &token);
    void sendTextMessage(int fromUid, int toUid, const QString &content);
    void sendImageMessage(int fromUid, int toUid, const QString &base64Data,
                         const QString &imageType, int width, int height, int fileSize);
    void sendGroupTextMessage(int fromUid, int groupId, const QString &content);
    void sendGroupImageMessage(int fromUid, int groupId, const QString &base64Data,
                              const QString &imageType, int width, int height, int fileSize);
    void sendAddFriendRequest(int fromUid, int toUid, const QString &message);
    void sendAuthFriendResponse(int fromUid, int toUid, bool accepted);
    void sendHeartbeat();
    void sendCreateGroup(int ownerUid, const QString &name, const QString &avatar);
    void sendAddGroupMember(int groupId, int memberUid);
    void sendRemoveGroupMember(int groupId, int memberUid);

signals:
    void connected();
    void disconnected();
    void loginSuccess();
    void loginFailed(const QString &error);
    void textMessageReceived(int fromUid, const QString &fromName, const QString &fromAvatar,
                           const QString &content, const QString &time);
    void imageMessageReceived(int fromUid, const QString &fromName, const QString &fromAvatar,
                              const QString &base64Data, const QString &imageType,
                              int width, int height, int fileSize, const QString &time);
    void groupTextMessageReceived(int fromUid, const QString &fromName, const QString &fromAvatar,
                                   int groupId, const QString &groupName,
                                   const QString &content, const QString &time);
    void groupImageMessageReceived(int fromUid, const QString &fromName, const QString &fromAvatar,
                                   int groupId, const QString &groupName,
                                   const QString &base64Data, const QString &imageType,
                                   int width, int height, int fileSize, const QString &time);
    void friendRequestReceived(int fromUid, const QString &fromName, const QString &fromAvatar,
                              const QString &message, const QString &requestTime);
    void friendAuthResponseReceived(int requestId, int fromUid, const QString &fromName,
                                    bool accepted, const QString &message);
    void errorOccurred(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);
    void onHeartbeatTimeout();

private:
    void readMessage();
    void parseMessage(quint16 msgId, const QJsonObject &json);
    QByteArray encodeMessage(quint16 msgId, const QJsonObject &json);

    QTcpSocket *m_socket;
    QTimer *m_heartbeatTimer;
    QByteArray m_recvBuffer;
    bool m_isAuthenticated;
    int m_uid;
};

#endif // TCPCLIENT_H
