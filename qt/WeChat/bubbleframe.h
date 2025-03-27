#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H

#include <QFrame>
#include "global.h"
#include <QHBoxLayout>

class BubbleFrame:public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(MessageType type,QWidget *parent=nullptr);
    void setMargin(int margin);
    void setWidget(QWidget *w);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QHBoxLayout *_layout;
    MessageType _type;
    int _margin;
};

#endif // BUBBLEFRAME_H
