#include "bubbleframe.h"
#include <QPainter>

#define WIDTH_TRIANGLE 5

BubbleFrame::BubbleFrame(MessageType type,QWidget *parent):_type(type),QFrame(parent), _margin(3){
    _layout = new QHBoxLayout(this);
    if(_type==MessageType::Send){
        _layout->setContentsMargins(_margin,_margin,5+_margin,_margin);
    }else{
        _layout->setContentsMargins(5+_margin,_margin,_margin,_margin);
    }
    this->setLayout(_layout);
}

void BubbleFrame::setWidget(QWidget *w)
{
    if(_layout->count()>0){
        return;
    }else{
        _layout->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    if(_type==MessageType::Send){
        QColor background_color(255,255,255);
        painter.setBrush(QBrush(background_color));
        QRect rect=QRect(WIDTH_TRIANGLE,0,this->width()-WIDTH_TRIANGLE,this->height());
        painter.drawRoundedRect(rect,5,5);
        painter.setBrush(QBrush(background_color));
        QPoint points[3] = {
            QPoint(WIDTH_TRIANGLE,this->height()/2-5),
            QPoint(0,this->height()/2),
            QPoint(WIDTH_TRIANGLE,this->height()/2+5)
        };
    }else{
        QColor background_color("#89d961");
        painter.setBrush(QBrush(background_color));
        QRect rect=QRect(0,0,this->width()-WIDTH_TRIANGLE,this->height());
        painter.drawRoundedRect(rect,5,5);
        painter.setBrush(QBrush(background_color));
        QPoint points[3] = {
            QPoint(this->width()-WIDTH_TRIANGLE,this->height()/2-5),
            QPoint(this->width(),this->height()/2),
            QPoint(this->width()-WIDTH_TRIANGLE,this->height()/2+5)
        };
        painter.drawPolygon(points,3);
    }
}

