#include "chatpagewidget.h"
#include "ui_chatpagewidget.h"

chatPageWidget::chatPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatPageWidget)
{
    ui->setupUi(this);
}

chatPageWidget::~chatPageWidget()
{
    delete ui;
}
