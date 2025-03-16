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
    void setInfo(const QString& name,const QString& headPath,const QDateTime& time,const QString& msg);
private:
    Ui::ChatUserWidget *ui;
    QString _name;
    QString _headPath;
    QDateTime  _time;
    QString _msg;
};

#endif // CHATUSERWIDGET_H
