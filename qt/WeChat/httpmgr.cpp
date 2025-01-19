#include "httpmgr.h"

//析构函数
HttpMgr::~HttpMgr()
{

}

//构造函数
HttpMgr::HttpMgr() {
    connect(this,&HttpMgr::signal_http_finished,this,&HttpMgr::slot_http_finished);
}

void HttpMgr::postHttpRequest(const QUrl &url, QJsonObject &json, ReqId req_id, Modules mod)
{
    //获取发送的数据
    QJsonDocument jsonDoc(json);
    QByteArray data=jsonDoc.toJson();//将json对象转换为字节数组

    //构建请求头
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");//设置请求目标
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.size()));//设置请求数据长度

    //发送并接收回应
    auto self=shared_from_this();//共用引用计数
    QNetworkReply *reply= _manager.post(request,data);
    connect(reply,&QNetworkReply::finished,[self,reply,req_id,mod](){
        //处理错误
        if(reply->error()!=QNetworkReply::NoError){
            qDebug()<<"网络错误:"<<reply->errorString();
            //发送错误信号
            emit self->signal_http_finished(req_id,mod,"",ErrorCodes::ERR_NETWORK);
            reply->deleteLater();//延迟回收
            return;
        }

        //读取回复
        QString res=reply->readAll();
        //发送完成信号
        emit self->signal_http_finished(req_id,mod,res,ErrorCodes::SUCCESS);
        reply->deleteLater();//延迟回收
        return;
    });
}

//处理http请求完成的槽函数
void HttpMgr::slot_http_finished(ReqId req_id, Modules mod, QString res, ErrorCodes ec)
{
    if(mod==Modules::REGISTERMOD){
        //发送注册模块完成信号
        emit signal_mod_register_finished(req_id,res,ec);
    }
}
