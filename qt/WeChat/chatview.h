#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QTimer>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget * parent=nullptr);
};

#endif // CHATVIEW_H
