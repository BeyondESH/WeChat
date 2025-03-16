#ifndef CHATUSERWIDGET_H
#define CHATUSERWIDGET_H

#include <QWidget>

namespace Ui {
class ChatUserWidget;
}

class ChatUserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatUserWidget(QWidget *parent = nullptr);
    ~ChatUserWidget();

private:
    Ui::ChatUserWidget *ui;
};

#endif // CHATUSERWIDGET_H
