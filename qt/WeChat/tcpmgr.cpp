#include "tcpmgr.h"

TCPMgr::TCPMgr():_isPending(false)
{
    connect(&_socket,&QTcpSocket::connected,[&](){
        qDebug()<<"连接成功";
        emit signal_tcp_connected(true);
    });

    connect(&_socket,&QTcpSocket::readyRead,[&](){
        _buffer.append(_socket.readAll());//读取数据
        QDataStream stream(&_buffer,QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Version::Qt_6_8);
        while(true){
            if(!_isPending){
                //解释消息头
                if(_buffer.size()<static_cast<int>(sizeof(quint16)*2)){
                    //消息头不完整
                    qDebug()<<"消息头不完整";
                    return;
                }
                stream>>_msgId>>_msgLen;
                // 输出消息头
                qDebug() << "Message ID:" << _msgId << ", Length:" << _msgLen;
                _buffer.remove(0,sizeof(quint16)*2);
                _buffer.squeeze();
            }
            if(_buffer.size()-sizeof(quint16)*2<_msgLen){
                //消息体不完整
                _isPending=true;
                return;
            }
            _isPending=false;
            QByteArray body=_buffer.left(_msgLen);
            _buffer.remove(0,_msgLen);
            _buffer.squeeze();
            handleMsg(static_cast<ReqId>(_msgId),_msgLen,body);
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
            break;
        case QAbstractSocket::RemoteHostClosedError:
            qDebug() << "The remote host closed the connection.";
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug() << "The host was not found.";
            break;
        case QAbstractSocket::SocketTimeoutError:
            qDebug() << "The socket operation timed out.";
            break;
        default:
            qDebug() << "An unidentified error occurred.";
            break;
        }
    });

    connect(&_socket,&QTcpSocket::disconnected,[](){
        qDebug()<<"与客户端断开连接";
    });
    initHandlers();
}

void TCPMgr::initHandlers()
{
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP,[this](ReqId id,int len,QByteArray data){

    });
}

void TCPMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    _handlers[id](id,len,data);
}
