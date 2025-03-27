#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H
#include "global.h"
#include <QTextEdit>
class MessageTextEdit:public QTextEdit
{
public:
    MessageTextEdit(QWidget *w);
    ~MessageTextEdit();
signals:
    void send();
protected:
};

#endif // MESSAGETEXTEDIT_H
