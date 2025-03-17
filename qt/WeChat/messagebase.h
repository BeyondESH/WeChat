#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include "listitembase.h"

class MessageBase:public ListItemBase
{
    Q_OBJECT
public:
    MessageBase(QWidget *parent = nullptr);
};

#endif // MESSAGEBASE_H
