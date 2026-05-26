#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <functional>
#include "singleton.h"

class HttpClient : public QObject, public Singleton<HttpClient>
{
    Q_OBJECT

public:
    explicit HttpClient(QObject *parent = nullptr);
    ~HttpClient();

    void setBaseUrl(const QString &url);

    void registerUser(const QString &email, const QString &verifyCode,
                     const QString &userName, const QString &password,
                     std::function<void(bool, const QString&)> callback);

    void loginByAccount(const QString &userName, const QString &password,
                       std::function<void(bool, int, const QString&, const QString&, int, const QString&)> callback);

    void loginByEmail(const QString &email, const QString &password,
                     std::function<void(bool, int, const QString&, const QString&, int, const QString&)> callback);

    void getVerifyCode(const QString &email, std::function<void(bool)> callback);

    void resetPassword(const QString &email, const QString &verifyCode,
                      const QString &newPassword, std::function<void(bool)> callback);

    void getFriendList(int uid, const QString &token,
                      std::function<void(const QJsonArray&)> callback);

    void searchUser(const QString &keyword, int searchType, const QString &token,
                   std::function<void(const QJsonArray&)> callback);

    void getChatHistory(int uid, int contactUid, int count, int offset, const QString &token,
                       std::function<void(const QJsonArray&)> callback);

    void getGroupList(int uid, const QString &token,
                     std::function<void(const QJsonArray&)> callback);

    void getGroupMembers(int groupId, const QString &token,
                        std::function<void(const QJsonArray&)> callback);

    void getGroupChatHistory(int uid, int groupId, int count, int offset, const QString &token,
                            std::function<void(const QJsonArray&)> callback);

    void createGroup(int ownerUid, const QString &name, const QString &avatar,
                    const QString &token, std::function<void(bool, int)> callback);

    void addGroupMember(int groupId, int memberUid, const QString &token,
                       std::function<void(bool)> callback);

signals:
    void requestFinished(bool success, const QString &error);

private slots:
    void onRequestFinished();

private:
    void sendPostRequest(const QString &path, const QJsonObject &json,
                        std::function<void(bool, const QJsonObject&)> callback);
    void sendGetRequest(const QString &path, std::function<void(bool, const QJsonObject&)> callback);

    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;
    QMap<QNetworkReply*, std::function<void(bool, const QJsonObject&)> > m_pendingCallbacks;
};

#endif // HTTPCLIENT_H
