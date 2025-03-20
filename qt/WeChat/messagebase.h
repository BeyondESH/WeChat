#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include "listitembase.h"

class MessageBase:public ListItemBase
{
    Q_OBJECT
public:
    MessageBase(QWidget *parent = nullptr);
protected:
    MessageType _msgType;
    MessageContentType _contentType;
    MessageStatus _msgStatus;
    QString _msgContent;
    QString _msgTime;
    QString _msgSender;
    QString _msgReceiver;
};

#endif // MESSAGEBASE_H
