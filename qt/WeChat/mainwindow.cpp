#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _loginDialog=new LoginDialog(this);
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_loginDialog);
    connect(_loginDialog, &LoginDialog::registerPBClicked, this, &MainWindow::openRegisterDialog);
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
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_loginDialog);
    connect(_loginDialog, &LoginDialog::registerPBClicked, this, &MainWindow::openRegisterDialog);
}

void MainWindow::openRegisterDialog()
{
    _registerDialog=new RegisterDialog(this);
    _registerDialog->hide();
    _registerDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_registerDialog);
    connect(_registerDialog, &RegisterDialog::backPBClicked, this, &MainWindow::openLoginDialog);
}
