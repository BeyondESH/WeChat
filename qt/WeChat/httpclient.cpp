#include "httpclient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8888")
{
}

HttpClient::~HttpClient()
{
}

void HttpClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
}

void HttpClient::registerUser(const QString &email, const QString &verifyCode,
                              const QString &userName, const QString &password,
                              std::function<void(bool, const QString&)> callback)
{
    QJsonObject json;
    json["email"] = email;
    json["verifyCode"] = verifyCode;
    json["user"] = userName;
    json["password"] = password;

    sendPostRequest("/user_register", json, [callback](bool success, const QJsonObject &response) {
        if (success) {
            int error = response["error"].toInt();
            callback(error == 0, QString("Error code: %1").arg(error));
        } else {
            callback(false, "Network error");
        }
    });
}

void HttpClient::loginByAccount(const QString &userName, const QString &password,
                               std::function<void(bool, int, const QString&, const QString&, int, const QString&)> callback)
{
    QJsonObject json;
    json["user"] = userName;
    json["password"] = password;

    sendPostRequest("/account_login", json, [callback](bool success, const QJsonObject &response) {
        if (success) {
            int error = response["error"].toInt();
            if (error == 0) {
                int uid = response["uid"].toInt();
                QString token = response["token"].toString();
                QString host = response["host"].toString();
                int port = response["port"].toInt();
                QString userName = response.value("userName").toString();
                callback(true, uid, token, host, port, userName);
            } else {
                callback(false, 0, "", "", 0, "");
            }
        } else {
            callback(false, 0, "", "", 0, "");
        }
    });
}

void HttpClient::loginByEmail(const QString &email, const QString &password,
                             std::function<void(bool, int, const QString&, const QString&, int, const QString&)> callback)
{
    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    sendPostRequest("/email_login", json, [callback](bool success, const QJsonObject &response) {
        if (success) {
            int error = response["error"].toInt();
            if (error == 0) {
                int uid = response["uid"].toInt();
                QString token = response["token"].toString();
                QString host = response["host"].toString();
                int port = response["port"].toInt();
                QString userName = response.value("userName").toString();
                callback(true, uid, token, host, port, userName);
            } else {
                callback(false, 0, "", "", 0, "");
            }
        } else {
            callback(false, 0, "", "", 0, "");
        }
    });
}

void HttpClient::getVerifyCode(const QString &email, std::function<void(bool)> callback)
{
    QJsonObject json;
    json["email"] = email;

    sendPostRequest("/get_varifycode", json, [callback](bool success, const QJsonObject &response) {
        if (success) {
            int error = response["error"].toInt();
            callback(error == 0);
        } else {
            callback(false);
        }
    });
}

void HttpClient::resetPassword(const QString &email, const QString &verifyCode,
                              const QString &newPassword, std::function<void(bool)> callback)
{
    QJsonObject json;
    json["email"] = email;
    json["verifyCode"] = verifyCode;
    json["password"] = newPassword;

    sendPostRequest("/reset_password", json, [callback](bool success, const QJsonObject &response) {
        if (success) {
            int error = response["error"].toInt();
            callback(error == 0);
        } else {
            callback(false);
        }
    });
}

void HttpClient::getFriendList(int uid, const QString &token,
                              std::function<void(const QJsonArray&)> callback)
{
    Q_UNUSED(uid)
    Q_UNUSED(token)

    QJsonObject json;
    json["uid"] = uid;
    json["token"] = token;

    sendPostRequest("/friend_list", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(response["friends"].toArray());
        } else {
            callback(QJsonArray());
        }
    });
}

void HttpClient::searchUser(const QString &keyword, int searchType, const QString &token,
                          std::function<void(const QJsonArray&)> callback)
{
    QJsonObject json;
    json["keyword"] = keyword;
    json["searchType"] = searchType;
    json["token"] = token;

    sendPostRequest("/search_user", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(response["users"].toArray());
        } else {
            callback(QJsonArray());
        }
    });
}

void HttpClient::getChatHistory(int uid, int contactUid, int count, int offset, const QString &token,
                               std::function<void(const QJsonArray&)> callback)
{
    QJsonObject json;
    json["uid"] = uid;
    json["contactUid"] = contactUid;
    json["count"] = count;
    json["offset"] = offset;
    json["token"] = token;

    sendPostRequest("/get_chat_history", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(response["messages"].toArray());
        } else {
            callback(QJsonArray());
        }
    });
}

void HttpClient::getGroupList(int uid, const QString &token,
                             std::function<void(const QJsonArray&)> callback)
{
    QJsonObject json;
    json["uid"] = uid;
    json["token"] = token;

    sendPostRequest("/get_group_list", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(response["groups"].toArray());
        } else {
            callback(QJsonArray());
        }
    });
}

void HttpClient::getGroupMembers(int groupId, const QString &token,
                                std::function<void(const QJsonArray&)> callback)
{
    QJsonObject json;
    json["groupId"] = groupId;
    json["token"] = token;

    sendPostRequest("/get_group_members", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(response["members"].toArray());
        } else {
            callback(QJsonArray());
        }
    });
}

void HttpClient::getGroupChatHistory(int uid, int groupId, int count, int offset, const QString &token,
                                    std::function<void(const QJsonArray&)> callback)
{
    QJsonObject json;
    json["uid"] = uid;
    json["groupId"] = groupId;
    json["count"] = count;
    json["offset"] = offset;
    json["token"] = token;

    sendPostRequest("/get_group_chat_history", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(response["messages"].toArray());
        } else {
            callback(QJsonArray());
        }
    });
}

void HttpClient::createGroup(int ownerUid, const QString &name, const QString &avatar,
                            const QString &token, std::function<void(bool, int)> callback)
{
    QJsonObject json;
    json["ownerUid"] = ownerUid;
    json["name"] = name;
    json["avatar"] = avatar;
    json["token"] = token;

    sendPostRequest("/create_group", json, [callback](bool success, const QJsonObject &response) {
        if (success && response["error"].toInt() == 0) {
            callback(true, response["groupId"].toInt());
        } else {
            callback(false, 0);
        }
    });
}

void HttpClient::addGroupMember(int groupId, int memberUid, const QString &token,
                               std::function<void(bool)> callback)
{
    QJsonObject json;
    json["groupId"] = groupId;
    json["memberUid"] = memberUid;
    json["token"] = token;

    sendPostRequest("/add_group_member", json, [callback](bool success, const QJsonObject &response) {
        if (success) {
            int error = response["error"].toInt();
            callback(error == 0);
        } else {
            callback(false);
        }
    });
}

void HttpClient::sendPostRequest(const QString &path, const QJsonObject &json,
                                std::function<void(bool, const QJsonObject&)> callback)
{
    QUrl url(m_baseUrl + path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(json).toJson());

    m_pendingCallbacks[reply] = callback;
    connect(reply, &QNetworkReply::finished, this, &HttpClient::onRequestFinished);
}

void HttpClient::onRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    auto callbackIt = m_pendingCallbacks.find(reply);
    if (callbackIt != m_pendingCallbacks.end()) {
        bool success = false;
        QJsonObject response;

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                response = doc.object();
                success = true;
            }
        } else {
            qDebug() << "Network error:" << reply->errorString();
        }

        callbackIt.value()(success, response);
        m_pendingCallbacks.erase(callbackIt);
    }

    reply->deleteLater();
}
