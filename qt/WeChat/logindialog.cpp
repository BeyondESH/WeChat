#include "logindialog.h"
#include "ui_logindialog.h"
#include "global.h"
#include "httpmgr.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    polish();//刷新qss
    connect(HttpMgr::getInstance().get(),&HttpMgr::signal_mod_login_finished,this,&LoginDialog::slot_mod_login_finished);
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
    if(ui->accountLineEdit->text().isEmpty()){
        ui->accountNodeLabel->setText("账号不能为空");
        ui->accountNodeLabel->setProperty("state","error");
        repolish(ui->accountNodeLabel);
        return;
    }
    if(ui->passwordLineEdit->text().isEmpty()){
        ui->passwordNodeLabel->setText("密码不能为空");
        ui->passwordNodeLabel->setProperty("state","error");
        repolish(ui->passwordNodeLabel);
        return;
    }
    QString password=ui->passwordLineEdit->text();
    //判断密码格式是否正确
    QRegularExpression rePWD("^[A-Za-z0-9]{8,20}$");//密码由8~20个数字或字母组成
    bool matchPWD=rePWD.match(password).hasMatch();
    if(!matchPWD){
        ui->passwordNodeLabel->setText("密码格式错误");
        ui->passwordNodeLabel->setProperty("state","error");
        repolish(ui->passwordNodeLabel);
        return;
    }
    QString user=ui->accountLineEdit->text();
    QJsonObject jsonObj;
    jsonObj["password"]=stringToSha256(password);
    QRegularExpression reAccount("^[a-zA-Z0-9]{8,16}$");//账号由8~16个数字或字母组成
    bool matchAccount=reAccount.match(user).hasMatch();
    QRegularExpression reEmail("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");//正确的邮箱格式
    bool matchEmail=reEmail.match(user).hasMatch();
    if(matchAccount){//账号登录
        jsonObj["user"]=user;
        //qDebug()<<"账号登录";
        HttpMgr::getInstance()->postHttpRequest(QUrl(gate_url_prefix+"/account_login"),jsonObj,ReqId::ID_LOGIN,Modules::LOGINMOD);
    }else if(matchEmail){//邮箱登录
        jsonObj["email"]=user;
        //qDebug()<<"邮箱登录";
        HttpMgr::getInstance()->postHttpRequest(QUrl(gate_url_prefix+"/email_login"),jsonObj,ReqId::ID_LOGIN,Modules::LOGINMOD);
    }else{
        ui->accountNodeLabel->setText("账号或邮箱格式错误");
        ui->accountNodeLabel->setProperty("state","error");
        repolish(ui->accountNodeLabel);
        return;
    }

}


void LoginDialog::on_resetPasswordPB_clicked()
{
    emit resetPasswordPBClicked();
}

void LoginDialog::slot_mod_login_finished(ReqId req_id,QString res,ErrorCodes ec){
    qDebug()<<"login";
}

