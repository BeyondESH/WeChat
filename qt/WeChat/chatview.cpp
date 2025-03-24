#include "chatview.h"
#include <QScrollBar>
#include <QTimer>
ChatView::ChatView(QWidget *parent):QWidget(parent),isLoading(false)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(0,0,0,0);

    _scrollArea=new QScrollArea(this);
    _scrollArea->setObjectName("chatArea");
    mainLayout->addWidget(_scrollArea);

    QWidget* w=new QWidget(this);
    w->setObjectName("chatBG");
    w->setAutoFillBackground(true);

    QVBoxLayout* vLayout = new QVBoxLayout(w);
    vLayout->addWidget(new QWidget(this),100000);
    w->setLayout(vLayout);
    _scrollArea->setWidget(w);

    _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar *verticalScrollBar = _scrollArea->verticalScrollBar();
    connect(verticalScrollBar, &QScrollBar::rangeChanged, this, &ChatView::onVScrollBarMoved);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if(isLoading){
        QScrollBar *verticalScrollBar = _scrollArea->verticalScrollBar();
        verticalScrollBar->setSliderPosition(verticalScrollBar->maximum());
        QTimer::singleShot(500,[this](){
            isLoading=false;
        });
    }
}
