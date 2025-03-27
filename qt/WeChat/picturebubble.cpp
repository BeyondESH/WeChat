#include "picturebubble.h"
#include <QLabel>

#define PIC_WIDTH 150
#define PIC_HEIGHT 100

PictureBubble::PictureBubble(const QPixmap &picture, MessageType type, QWidget *parent):BubbleFrame(type,parent)
{
    QLabel *label=new QLabel(this);
    label->setScaledContents(true);
    QPixmap pix = picture.scaled(QSize(PIC_WIDTH, PIC_HEIGHT), Qt::KeepAspectRatio);
    label->setPixmap(pix);
    this->setWidget(label);
    int left_margin = this->layout()->contentsMargins().left();
    int right_margin = this->layout()->contentsMargins().right();
    int v_margin = this->layout()->contentsMargins().bottom();
    setFixedSize(pix.width()+left_margin + right_margin, pix.height() + v_margin *2);
}
