#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QScrollEvent>
#include <QDebug>

class ChatUserList:public QListWidget
{
    Q_OBJECT
protected:
    //bool eventFilter(QObject *watched, QEvent *event) override;

public:
    ChatUserList(QWidget *parent = nullptr);

signals:
    void signal_loading_chat_user();
};

#endif // CHATUSERLIST_H
