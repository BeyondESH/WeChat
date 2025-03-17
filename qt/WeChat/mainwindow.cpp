#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,&MainWindow::signal_set_centralWidget,this,&MainWindow::slot_set_centralWidget);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);// 设置主窗口为无边框和隐藏按钮
    this->setStyleSheet("QMainWindow { border-radius: 10px; background-color: white; }");//设置窗口圆角
    _loginDialog=new LoginDialog(this);
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _loginDialog->setStyleSheet("QDialog { border-radius: 10px; background-color: white; }");//设置窗口圆角
    setCentralWidget(_loginDialog);
    adjustSize();
    connect(_loginDialog, &LoginDialog::registerPBClicked, this, &MainWindow::openRegisterDialog);
    connect(_loginDialog,&LoginDialog::resetPasswordPBClicked,this,&MainWindow::openResetDialog);
    connect(TCPMgr::getInstance().get(),&TCPMgr::signal_switch_chatDialog,this,&MainWindow::slot_switch_chatDialog);
    //emit TCPMgr::getInstance()->signal_switch_chatDialog();
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow调用了析构函数";
    delete ui;
}

void MainWindow::setCentralWidget(QWidget *widget)
{
    QMainWindow::setCentralWidget(widget);
    emit signal_set_centralWidget(widget);
}

void MainWindow::openLoginDialog()
{
    //qDebug()<<openLoginDialog;
    _loginDialog=new LoginDialog(this);
    _loginDialog->hide();
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_loginDialog);
    adjustSize();
    connect(_loginDialog, &LoginDialog::registerPBClicked, this, &MainWindow::openRegisterDialog);
    connect(_loginDialog,&LoginDialog::resetPasswordPBClicked,this,&MainWindow::openResetDialog);
}

void MainWindow::openRegisterDialog()
{
    _registerDialog=new RegisterDialog(this);
    _registerDialog->hide();
    _registerDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_registerDialog);
    adjustSize();
    connect(_registerDialog, &RegisterDialog::backPBClicked, this, &MainWindow::openLoginDialog);
}

void MainWindow::openResetDialog()
{
    //qDebug()<<12;
    _resetDialog=new ResetDialog(this);
    _resetDialog->hide();
    _resetDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_resetDialog);
    adjustSize();
    connect(_resetDialog, &ResetDialog::backPBClicked, this, &MainWindow::openLoginDialog);
}

void MainWindow::slot_switch_chatDialog()
{
    _loginDialog->showTip(tr("登录成功"),true);
    _loginDialog->close();
    _chatDialog=new ChatDialog;
    _chatDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_chatDialog);
    adjustSize();
}

void MainWindow::slot_set_centralWidget(QWidget *widget)
{
    if(widget==_loginDialog)
    {
        setMinimumSize(320, 400);
    }
    else if(widget==_registerDialog)
    {
        setMinimumSize(350, 500);
    }
    else if(widget==_resetDialog)
    {
        setMinimumSize(350, 500);
    }else if(widget==_chatDialog){
        setMinimumSize(1065, 820);
        setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    }
}
