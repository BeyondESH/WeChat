#include "chatview.h"
#include <QScrollBar>
#include <QTimer>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>


ChatView::ChatView(QWidget *parent):QWidget(parent),_isLoading(false)
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

    QHBoxLayout* hLayout=new QHBoxLayout(this);
    hLayout->addWidget(verticalScrollBar,0,Qt::AlignRight);
    hLayout->setContentsMargins(0,0,0,0);
    _scrollArea->setLayout(hLayout);
    verticalScrollBar->setVisible(false);
    _scrollArea->setWidgetResizable(true);
    _scrollArea->installEventFilter(this);
    initStyleSheet();
}

void ChatView::appendChatItem(QWidget *item)
{
    QVBoxLayout* vLayout=qobject_cast<QVBoxLayout *>(_scrollArea->widget()->layout());
    vLayout->insertWidget(vLayout->count()-1,item);
    _isLoading=true;
}

void ChatView::prependChatItem(QWidget *item)
{

}

void ChatView::insertChatItem(QWidget *before, QWidget *item)
{
}

bool ChatView::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==_scrollArea){
        if(event->type()==QEvent::Enter){
            _scrollArea->verticalScrollBar()->setHidden(_scrollArea->verticalScrollBar()->maximum()==0);

        }else if(event->type()==QEvent::Leave){
            _scrollArea->verticalScrollBar()->setHidden(true);
        }
    }

    return QWidget::eventFilter(watched,event);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if(_isLoading){
        QScrollBar *verticalScrollBar = _scrollArea->verticalScrollBar();
        verticalScrollBar->setSliderPosition(verticalScrollBar->maximum());
        QTimer::singleShot(500,[this](){
            _isLoading=false;
        });
    }
}

void ChatView::initStyleSheet()
{

}
