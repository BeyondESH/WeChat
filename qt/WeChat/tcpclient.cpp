#include "tcpclient.h"
#include <QHostAddress>
#include <QJsonArray>
#include <QDebug>

#define MSG_ID_SIZE 2
#define MSG_LEN_SIZE 2
#define MSG_HEAD_SIZE (MSG_ID_SIZE + MSG_LEN_SIZE)

TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_isAuthenticated(false)
    , m_uid(0)
{
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &TcpClient::onError);

    connect(m_heartbeatTimer, &QTimer::timeout, this, &TcpClient::onHeartbeatTimeout);
    m_heartbeatTimer->setInterval(30000);
}

TcpClient::~TcpClient()
{
    m_heartbeatTimer->stop();
    m_socket->disconnectFromHost();
}

void TcpClient::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(QHostAddress(host), port);
}

void TcpClient::disconnectFromServer()
{
    m_heartbeatTimer->stop();
    m_isAuthenticated = false;
    m_socket->disconnectFromHost();
}

void TcpClient::sendLoginRequest(int uid, const QString &token)
{
    QJsonObject json;
    json["uid"] = uid;
    json["token"] = token;

    QByteArray data = encodeMessage(1005, json);
    m_socket->write(data);

    m_uid = uid;
}

void TcpClient::sendTextMessage(int fromUid, int toUid, const QString &content)
{
    QJsonObject json;
    json["fromUid"] = fromUid;
    json["toUid"] = toUid;
    json["content"] = content;

    QByteArray data = encodeMessage(1017, json);
    m_socket->write(data);
}

void TcpClient::sendImageMessage(int fromUid, int toUid, const QString &base64Data,
                               const QString &imageType, int width, int height, int fileSize)
{
    QJsonObject json;
    json["fromUid"] = fromUid;
    json["toUid"] = toUid;
    json["content"] = base64Data;
    json["imageType"] = imageType;
    json["width"] = width;
    json["height"] = height;
    json["fileSize"] = fileSize;

    QByteArray data = encodeMessage(1028, json);
    m_socket->write(data);
}

void TcpClient::sendGroupTextMessage(int fromUid, int groupId, const QString &content)
{
    QJsonObject json;
    json["fromUid"] = fromUid;
    json["groupId"] = groupId;
    json["content"] = content;

    QByteArray data = encodeMessage(2007, json);
    m_socket->write(data);
}

void TcpClient::sendGroupImageMessage(int fromUid, int groupId, const QString &base64Data,
                                    const QString &imageType, int width, int height, int fileSize)
{
    QJsonObject json;
    json["fromUid"] = fromUid;
    json["groupId"] = groupId;
    json["content"] = base64Data;
    json["imageType"] = imageType;
    json["width"] = width;
    json["height"] = height;
    json["fileSize"] = fileSize;

    QByteArray data = encodeMessage(2016, json);
    m_socket->write(data);
}

void TcpClient::sendAddFriendRequest(int fromUid, int toUid, const QString &message)
{
    QJsonObject json;
    json["fromUid"] = fromUid;
    json["toUid"] = toUid;
    json["message"] = message;

    QByteArray data = encodeMessage(1009, json);
    m_socket->write(data);
}

void TcpClient::sendAuthFriendResponse(int fromUid, int toUid, bool accepted)
{
    QJsonObject json;
    json["fromUid"] = fromUid;
    json["toUid"] = toUid;
    json["accept"] = accepted;

    QByteArray data = encodeMessage(1013, json);
    m_socket->write(data);
}

void TcpClient::sendHeartbeat()
{
    QJsonObject json;
    QByteArray data = encodeMessage(1020, json);
    m_socket->write(data);
}

void TcpClient::sendCreateGroup(int ownerUid, const QString &name, const QString &avatar)
{
    QJsonObject json;
    json["ownerUid"] = ownerUid;
    json["name"] = name;
    json["avatar"] = avatar;

    QByteArray data = encodeMessage(2001, json);
    m_socket->write(data);
}

void TcpClient::sendAddGroupMember(int groupId, int memberUid)
{
    QJsonObject json;
    json["groupId"] = groupId;
    json["memberUid"] = memberUid;

    QByteArray data = encodeMessage(2003, json);
    m_socket->write(data);
}

void TcpClient::sendRemoveGroupMember(int groupId, int memberUid)
{
    QJsonObject json;
    json["groupId"] = groupId;
    json["uid"] = memberUid;

    QByteArray data = encodeMessage(2005, json);
    m_socket->write(data);
}

void TcpClient::onConnected()
{
    qDebug() << "Connected to server";
    emit connected();
}

void TcpClient::onDisconnected()
{
    qDebug() << "Disconnected from server";
    m_heartbeatTimer->stop();
    m_isAuthenticated = false;
    emit disconnected();
}

void TcpClient::onReadyRead()
{
    m_recvBuffer.append(m_socket->readAll());
    readMessage();
}

void TcpClient::readMessage()
{
    while (m_recvBuffer.size() >= MSG_HEAD_SIZE) {
        quint16 msgId;
        quint16 msgLen;

        QDataStream stream(m_recvBuffer);
        stream.setByteOrder(QDataStream::BigEndian);
        stream >> msgId >> msgLen;

        if (m_recvBuffer.size() < MSG_HEAD_SIZE + msgLen) {
            break;
        }

        QByteArray msgData = m_recvBuffer.mid(MSG_HEAD_SIZE, msgLen);
        m_recvBuffer.remove(0, MSG_HEAD_SIZE + msgLen);

        QJsonDocument doc = QJsonDocument::fromJson(msgData);
        if (doc.isObject()) {
            QJsonObject json = doc.object();
            parseMessage(msgId, json);
        }
    }
}

void TcpClient::parseMessage(quint16 msgId, const QJsonObject &json)
{
    switch (msgId) {
    case 1006: {
        int error = json["error"].toInt();
        if (error == 0) {
            m_isAuthenticated = true;
            m_heartbeatTimer->start();
            emit loginSuccess();
        } else {
            emit loginFailed(QString("Login failed: error code %1").arg(error));
        }
        break;
    }
    case 1018:
        break;
    case 1019:
        emit textMessageReceived(
            json["fromUid"].toInt(),
            json["fromName"].toString(),
            json["fromAvatar"].toString(),
            json["content"].toString(),
            json["time"].toString()
        );
        break;
    case 1021:
        break;
    case 1030:
        emit imageMessageReceived(
            json["fromUid"].toInt(),
            json["fromName"].toString(),
            json["fromAvatar"].toString(),
            json["content"].toString(),
            json["imageType"].toString(),
            json["width"].toInt(),
            json["height"].toInt(),
            json["fileSize"].toInt(),
            json["time"].toString()
        );
        break;
    case 1011:
        emit friendRequestReceived(
            json["fromUid"].toInt(),
            json["fromName"].toString(),
            json["fromAvatar"].toString(),
            json["message"].toString(),
            json["requestTime"].toString()
        );
        break;
    case 1015:
        emit friendAuthResponseReceived(
            json["requestId"].toInt(),
            json["fromUid"].toInt(),
            json["fromName"].toString(),
            json["accepted"].toBool(),
            json["message"].toString()
        );
        break;
    case 2008:
        break;
    case 2009:
        emit groupTextMessageReceived(
            json["fromUid"].toInt(),
            json["fromName"].toString(),
            json["fromAvatar"].toString(),
            json["groupId"].toInt(),
            json["groupName"].toString(),
            json["content"].toString(),
            json["time"].toString()
        );
        break;
    case 2018:
        emit groupImageMessageReceived(
            json["fromUid"].toInt(),
            json["fromName"].toString(),
            json["fromAvatar"].toString(),
            json["groupId"].toInt(),
            json["groupName"].toString(),
            json["content"].toString(),
            json["imageType"].toString(),
            json["width"].toInt(),
            json["height"].toInt(),
            json["fileSize"].toInt(),
            json["time"].toString()
        );
        break;
    default:
        qDebug() << "Unknown message id:" << msgId;
        break;
    }
}

QByteArray TcpClient::encodeMessage(quint16 msgId, const QJsonObject &json)
{
    QByteArray body = QJsonDocument(json).toJson(QJsonDocument::Compact);
    quint16 bodyLen = body.size();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << msgId << bodyLen;
    data.append(body);

    return data;
}

void TcpClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error:" << error << m_socket->errorString();
    emit errorOccurred(m_socket->errorString());
}

void TcpClient::onHeartbeatTimeout()
{
    if (m_isAuthenticated) {
        sendHeartbeat();
    }
}
