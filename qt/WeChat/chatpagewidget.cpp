#include "chatpagewidget.h"
#include "ui_chatpagewidget.h"
#include <QEvent>
#include <QMouseEvent>
ChatPageWidget::ChatPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatPageWidget)
{
    ui->setupUi(this);
}

ChatPageWidget::~ChatPageWidget()
{
    delete ui;
}

QWidget *ChatPageWidget::getTitleWidget() const
{
    return ui->titleWD;
}

void ChatPageWidget::on_quitPB_clicked()
{
    QApplication::quit();
}

void ChatPageWidget::on_minPB_clicked()
{
    this->parentWidget()->parentWidget()->parentWidget()->showMinimized();
}

void ChatPageWidget::on_maxPB_toggled(bool checked)
{
    auto pb = ui->maxPB;
    if (checked)
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max_clicked.svg"));
        this->parentWidget()->parentWidget()->parentWidget()->showMaximized();
    }
    else
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/max.svg"));
        this->parentWidget()->parentWidget()->parentWidget()->showNormal();
    }
}


void ChatPageWidget::on_topPB_toggled(bool checked)
{
    auto pb = ui->topPB;
    if (checked)
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/top_clicked.svg"));
    }
    else
    {
        pb->setIcon(QIcon(":/img/loginDialog/img/top.svg"));
    }
}
