#ifndef CHATUSERWIDGET_H
#define CHATUSERWIDGET_H

#include <QWidget>
#include "listitembase.h"
namespace Ui {
class ChatUserWidget;
}

class ChatUserWidget : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWidget(QWidget *parent = nullptr);
    ~ChatUserWidget();
    QSize sizeHint() const override;
    void setInfo(const QString& name,const QString& _pos,const QDate& time,const QString& msg);
private:
    Ui::ChatUserWidget *ui;
    QString _name;
    QString _pos;
    QDate _time;
    QString _msg;
};

#endif // CHATUSERWIDGET_H
