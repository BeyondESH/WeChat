#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include <QHBoxLayout>
#include <QPixmap>
#include "BubbleFrame.h"

class PictureBubble:public BubbleFrame
{
public:
    PictureBubble(const QPixmap &picture,MessageType type,QWidget *parent=nullptr);
};

#endif // PICTUREBUBBLE_H
