#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _loginDialog=new LoginDialog(this);
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setCentralWidget(_loginDialog);
    adjustSize();
    connect(_loginDialog, &LoginDialog::registerPBClicked, this, &MainWindow::openRegisterDialog);
    connect(_loginDialog,&LoginDialog::resetPasswordPBClicked,this,&MainWindow::openResetDialog);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow调用了析构函数";
    delete ui;
}

void MainWindow::openLoginDialog()
{
    qDebug()<<1;
    _loginDialog=new LoginDialog(this);
    _loginDialog->hide();
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
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
