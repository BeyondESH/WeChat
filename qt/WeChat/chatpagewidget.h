#ifndef CHATPAGEWIDGET_H
#define CHATPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class chatPageWidget;
}

class chatPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit chatPageWidget(QWidget *parent = nullptr);
    ~chatPageWidget();

private:
    Ui::chatPageWidget *ui;
};

#endif // CHATPAGEWIDGET_H
