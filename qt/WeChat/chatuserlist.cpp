#include "chatuserlist.h"

// bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
// {

//     if(watched==this->viewport()){
//          //滚动条逻辑
//         if(event->type()==QEvent::Enter){
//             this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//         }else if(event->type()==QEvent::Leave){
//             this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//         }

//         //动态加载逻辑
//         if(event->type()==QEvent::Wheel){
//             QWheelEvent* wheelEvent=static_cast<QWheelEvent*>(event);
//         }
//     }

// }

ChatUserList::ChatUserList(QWidget *parent):QListWidget(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
