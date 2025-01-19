#include "logindialog.h"
#include "ui_logindialog.h"
#include "global.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    polish();//刷新qss
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"LoginDialog调用了析构函数";
    delete ui;
}

void LoginDialog::polish()
{
    ui->accountNodeLabel->setProperty("state","normal");
    ui->passwordNodeLabel->setProperty("state","normal");
    repolish(ui->accountNodeLabel);
    repolish(ui->passwordNodeLabel);
}



void LoginDialog::on_registerPushButton_clicked()
{
    emit registerPBClicked();
}


void LoginDialog::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1==Qt::Checked){
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}


void LoginDialog::on_loginButton_clicked()
{
    ui->passwordNodeLabel->clear();
    ui->accountNodeLabel->clear();
    if(ui->accountLineEdit->text().isEmpty()&&(!ui->passwordLineEdit->text().isEmpty())){
        ui->accountNodeLabel->setText("账号不能为空");
        ui->accountNodeLabel->setProperty("state","error");
        repolish(ui->accountNodeLabel);
    }else if(ui->passwordLineEdit->text().isEmpty()&&(!ui->accountLineEdit->text().isEmpty())){
        ui->passwordNodeLabel->setText("密码不能为空");
        ui->passwordNodeLabel->setProperty("state","error");
        repolish(ui->passwordNodeLabel);
    }else if(ui->accountLineEdit->text().isEmpty()&&ui->passwordLineEdit->text().isEmpty()){
        ui->accountNodeLabel->setText("账号不能为空");
        ui->accountNodeLabel->setProperty("state","error");
        repolish(ui->accountNodeLabel);
        ui->passwordNodeLabel->setText("密码不能为空");
        ui->passwordNodeLabel->setProperty("state","error");
        repolish(ui->passwordNodeLabel);
    }
}

