#ifndef HTTPMGR_H
#define HTTPMGR_H
/******************************************************************************
 *
 * @file       httpmgr.h
 * @brief      http请求管理器
 *
 * @author     李佳承
 * @date       2024/11/02
 * @history
 *****************************************************************************/

#include "singleton.hpp"//单例模板 里面包含global.h
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

class HttpMgr:public Singleton<HttpMgr>,public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();//析构函数
    void postHttpRequest(const QUrl& url,QJsonObject& json,ReqId req_id,Modules mod);//发送post请求
private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    HttpMgr(const HttpMgr&)=delete;
    HttpMgr&operator=(const HttpMgr&)=delete;
    QNetworkAccessManager _manager;
private slots:
    void slot_http_finished(ReqId id,Modules mod,QString res,ErrorCodes ec);
signals:
    void signal_http_finished(ReqId id,Modules mod,QString res,ErrorCodes ec);//http请求完成信号
    void signal_mod_register_finished(ReqId req_id,QString res,ErrorCodes ec);//注册模块完成信号
};

#endif // HTTPMGR_H
