#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include "messagebase.h"
namespace Ui {
class MessageWidget;
}

class MessageWidget : public MessageBase
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = nullptr);
    ~MessageWidget();

private:
    Ui::MessageWidget *ui;
};

#endif // MESSAGEWIDGET_H
