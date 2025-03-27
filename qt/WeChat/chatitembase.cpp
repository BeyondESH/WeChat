#include "chatitembase.h"

ChatItemBase::ChatItemBase(MessageType type, QWidget *parent)
{
    _iconLB = new QLabel(this);
    _iconLB->setScaledContents(true);
    _iconLB->setFixedSize(34,34);
    _bubbleWD=new QWidget(this);
    QGridLayout *layout = new QGridLayout();
    layout->setVerticalSpacing(3);
    layout->setHorizontalSpacing(3);
    layout->setContentsMargins(3,3,3,3);

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    if(_type==MessageType::Receive){
        layout->addWidget(_iconLB,0,0,1,1,Qt::AlignCenter);
        layout->addItem(spacer,0,2,1,1);
        layout->addWidget(_bubbleWD,0,1,1,1);
        layout->setColumnStretch(0,2);
        layout->setColumnStretch(1,3);
    }else{
        layout->addWidget(_iconLB,0,2,1,1,Qt::AlignCenter);
        layout->addItem(spacer,0,0,1,1);
        layout->addWidget(_bubbleWD,0,1,1,1);
        layout->setColumnStretch(1,2);
        layout->setColumnStretch(2,3);
    }
    this->setLayout(layout);
}

void ChatItemBase::setUserName(const QString &name)
{

}

void ChatItemBase::setUserIcon(const QString &iconPath)
{
    _iconLB->setPixmap(QPixmap(iconPath));
}

void ChatItemBase::setWidget(QWidget *w)
{
    QGridLayout *layout = dynamic_cast<QGridLayout*>(this->layout());
    layout->replaceWidget(_bubbleWD,w);
    delete _bubbleWD;
    _bubbleWD=w;
}


