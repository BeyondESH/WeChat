#include "tcpmgr.h"
#include "UserMgr.h"
TCPMgr::~TCPMgr()
{

}

TCPMgr::TCPMgr():_isPending(false)
{
    connect(&_socket,&QTcpSocket::connected,[&](){
        qDebug()<<"连接成功";
        emit signal_connected_success(true);
    });

    connect(&_socket,&QTcpSocket::readyRead,[&](){
        qDebug()<<"接收数据";
        _buffer.append(_socket.readAll());//读取数据
        //stream.setVersion(QDataStream::Version::Qt_6_8);
        while(_buffer.size() > 0){
            QDataStream stream(&_buffer,QIODevice::ReadOnly);
            if(!_isPending){
                //解释消息头
                if(_buffer.size()<static_cast<int>(sizeof(quint16)*2)){
                    //消息头不完整
                    qDebug()<<"消息头不完整";
                    return;
                }
                 const char* data = _buffer.constData();
                 _msgId = qFromBigEndian(*reinterpret_cast<const quint16*>(data));
                _msgLen = qFromBigEndian(*reinterpret_cast<const quint16*>(data + 2));
                qDebug() << "Message ID:" << _msgId << ", Length:" << _msgLen;
                _buffer.remove(0,sizeof(quint16)*2);
                _buffer.squeeze();
                _isPending=true;
            }
            if(_buffer.size()<_msgLen){
                //消息体不完整
                return;
            }
            QByteArray body=_buffer.left(_msgLen);
            _buffer.remove(0,_msgLen);
            _buffer.squeeze();
            handleMsg(static_cast<ReqId>(_msgId),_msgLen,body);//处理消息
            _isPending=false;
        }
    });

    //错误处理
    connect(&_socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError socketError) {
        // 使用 Q_UNUSED 宏来避免未使用参数的编译警告
        Q_UNUSED(socketError)
        // 输出详细的错误信息
        qDebug() << "Socket Error:" << _socket.errorString();
        // 可选：根据不同的错误类型执行不同的处理逻辑
        switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            qDebug() << "The connection was refused by the peer.";
            emit signal_connected_success(false);
            break;
        case QAbstractSocket::RemoteHostClosedError:
            qDebug() << "The remote host closed the connection.";
            emit signal_connected_success(false);
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug() << "The host was not found.";
            emit signal_connected_success(false);
            break;
        case QAbstractSocket::SocketTimeoutError:
            qDebug() << "The socket operation timed out.";
            emit signal_connected_success(false);
            break;
        default:
            qDebug() << "An unidentified error occurred.";
            emit signal_connected_success(false);
            break;
        }
    });

    connect(&_socket,&QTcpSocket::disconnected,[](){
        qDebug()<<"与客户端断开连接";
    });

    QObject::connect(this, &TCPMgr::signal_send_data, this, &TCPMgr::slot_send_data);

    initHandlers();
}

void TCPMgr::initHandlers()
{
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP,[this](ReqId id,int len,QByteArray data){
        QJsonDocument jsonDoc=QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            qDebug()<<tr("json解析失败");
            return;
        }
        QJsonObject jsonObj=jsonDoc.object();
        if(!jsonObj.contains("error")){
            qDebug()<<tr("token验证失败:")<<ErrorCodes::ERROR_JSON;
            emit signal_login_failed(ErrorCodes::ERROR_JSON);
            return;
        }
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS){
            qDebug()<<tr("token验证失败:")<<error;
            emit signal_login_failed(error);
            return;
        }
        qDebug()<<"token验证成功";
        UserMgr::getInstance()->setToken(jsonObj["token"].toString());
        emit signal_switch_chatDialog();
    });
}

void TCPMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    _handlers[id](id,len,data);
}

void TCPMgr::slots_tcp_connected(ServerInfo server)
{
    qDebug()<<"正在连接服务器";
    _host=server.host;
    _port=server.port.toUInt();
    _socket.connectToHost(_host,_port);
}

void TCPMgr::slot_send_data(ReqId reqId,QString string)
{
    quint16 id=static_cast<quint16>(reqId);
    QByteArray body=string.toUtf8();
    quint16 len=static_cast<quint16>(body.size());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream<<id<<len;
    data.append(body);
    _socket.write(data);
}
