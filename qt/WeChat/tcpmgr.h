#ifndef TCPMGR_H
#define TCPMGR_H
#include "singleton.hpp"
#include <qtcpsocket.h>
#include <QObject>
#include <functional>
#include "global.h"

class TCPMgr:public Singleton<TCPMgr>,public std::enable_shared_from_this<TCPMgr>
{
    Q_OBJECT
    friend class Singleton<TCPMgr>;
public:
    ~TCPMgr();
private:
    TCPMgr();
    TCPMgr(const TCPMgr&)=delete;
    TCPMgr&operator=(const TCPMgr&)=delete;
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _isPending;
    quint16 _msgId;
    quint16 _msgLen;
    void initHandlers();
    void handleMsg(ReqId id,int len,QByteArray data);
    QMap<ReqId,std::function<void(ReqId id,int len,QByteArray data)>> _handlers;
signals:
    void signal_tcp_connected();
    void signal_connected_success(bool isSucced);
    void signal_send_data(ReqId id,QString string);
    void signal_login_failed(int error);
    void signal_switch_chatDialog();
public slots:
    void slots_tcp_connected(ServerInfo info);
    void slot_send_data(ReqId reqId,QString string);
};

#endif // TCPMGR_H
