#include "chatuserwidget.h"
#include "ui_chatuserwidget.h"

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatUserWidget)
{
    ui->setupUi(this);
}

ChatUserWidget::~ChatUserWidget()
{
    delete ui;
}
