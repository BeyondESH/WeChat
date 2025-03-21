#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include "messagebase.h"
namespace Ui {
class MessageWidget;
}

class MessageWidget : public MessageBase
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = nullptr);
    ~MessageWidget();
    void setMsgType(const MessageType &type);
    MessageType getMsgType();
    void setMsgBody(const QString &body);
    QString getMsgBody();
    void setMsgTime(const QDateTime &time);
    QDateTime getMsgTime();
    void setMsgStatus(const MessageStatus &status);
    MessageStatus getMsgStatus();
    void setInfo(const MessageType& msgType,const QString& headPath,const QDateTime& time,const QString& msg);//时间
    void setInfo(const MessageType& msgType,const QString& headPath,const QString& msg);//不显时间
    void closeTime();
private:
    Ui::MessageWidget *ui;
    MessageType _msgType;
    QString _msgBody;
    QDateTime _msgTime;
    MessageStatus _msgStatus;
    QString _headPath;
    QWidget *_contentWidget;
};

#endif // MESSAGEWIDGET_H
