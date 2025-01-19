#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _loginDialog(new LoginDialog(this))  // 登录页面，作为MainWindow的子对象
    , _registerDialog(new RegisterDialog(this))  // 注册页面，作为MainWindow的子对象
{
    ui->setupUi(this);
    setCentralWidget(_loginDialog);
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _registerDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _registerDialog->hide();
    connect(_loginDialog, &LoginDialog::registerPBClicked, this, &MainWindow::openRegisterDialog);
    connect(_registerDialog, &RegisterDialog::backPBClicked, this, &MainWindow::openLoginDialog);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow调用了析构函数";
    delete ui;
}

void MainWindow::openLoginDialog()
{
    setCentralWidget(_loginDialog);
}

void MainWindow::openRegisterDialog()
{
    setCentralWidget(_registerDialog);
}
