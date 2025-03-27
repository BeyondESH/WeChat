#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H


#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"


class BubbleFrame;

class ChatItemBase:public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(MessageType type,QWidget* parent=nullptr);
    void setUserName(const QString &name);
    void setUserIcon(const QString &iconPath);
    void setWidget(QWidget *w);
private:
    MessageType _type;
    QLabel *_nameLB;
    QLabel *_iconLB;
    QWidget *_bubbleWD;
};

#endif // CHATITEMBASE_H
