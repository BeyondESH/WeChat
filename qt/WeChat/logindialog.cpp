#include "logindialog.h"
#include "ui_logindialog.h"
#include "global.h"
#include "httpmgr.h"
#include "tcpmgr.h"
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    polish();//刷新qss
    initHttpHandlers();
    connect(HttpMgr::getInstance().get(),&HttpMgr::signal_mod_login_finished,this,&LoginDialog::slot_mod_login_finished);
    connect(this,&LoginDialog::signal_tcp_connected,TCPMgr::getInstance().get(),&TCPMgr::slots_tcp_connected);
    connect(TCPMgr::getInstance().get(),&TCPMgr::signal_switch_chatDialog,this,&LoginDialog::slot_switch_chatDialog);
    connect(TCPMgr::getInstance().get(),&TCPMgr::signal_connected_success,this,&LoginDialog::slot_tcp_connect_finished);
    connect(TCPMgr::getInstance().get(),&TCPMgr::signal_login_failed,this,&LoginDialog::slot_login_failed);
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
    ui->errorlabel->clear();
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
    if(ec!=ErrorCodes::SUCCESS){
        showTip(tr("网络连接错误"),false);
        return;
    }
    //解析json字符串
    QByteArray byte=res.toUtf8();
    QJsonDocument jsonDoc=QJsonDocument::fromJson(byte);
    //json解析为空
    if(jsonDoc.isNull()){
        this->showTip(tr("Json解析失败"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject()){
        this->showTip(tr("Json解析失败"),false);
        return;
    }
    //json解析成功
    QJsonObject jsonObj=jsonDoc.object();
    _handlers[req_id](jsonObj);
    return;
}

void LoginDialog::slot_tcp_connect_finished(bool isSuccess)
{
    if(isSuccess==true){
        QJsonObject jsonObj;
        jsonObj["uid"]=_uid;
        jsonObj["token"]=_token;
        QJsonDocument jsonDoc(jsonObj);
        QString jsonStr=jsonDoc.toJson(QJsonDocument::Indented);
        TCPMgr::getInstance()->signal_send_data(ReqId::ID_CHAT_LOGIN,jsonStr);//发送登录信息给聊天服务器进行身份验证
    }else{
        showTip(tr("服务器连接失败"),false);
    }
}

void LoginDialog::slot_switch_chatDialog()
{
    qDebug()<<"切换至聊天界面";
}

void LoginDialog::slot_login_failed(int error)
{
    showTip(tr("登录验证失败"),false);
}

void LoginDialog::showTip(QString tip, bool isOK)
{
    ui->errorlabel->clear();
    if(isOK==true){
        ui->errorlabel->setProperty("state","normal");
    }else{
        ui->errorlabel->setProperty("state","error");
    }
    repolish(ui->errorlabel);
    ui->errorlabel->setText(tip);
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN,[this](const QJsonObject& jsonObj){
        const int error=jsonObj["error"].toInt();
        switch(error){
        case ErrorCodes::PasswordError:
            qDebug()<<"密码错误";
            showTip(tr("密码错误"),false);
            return;
            break;
        case ErrorCodes::ERR_MYSQL:
            qDebug()<<"mysql连接错误";
            showTip(tr("服务器连接错误"),false);
            return;
            break;
        case ErrorCodes::ERR_NETWORK:
            qDebug()<<"网络错误";
            showTip(tr("网络连接错误"),false);
            return;
            break;
        case ErrorCodes::UserNotExist:
            qDebug()<<"用户不存在";
            showTip(tr("用户不存在"),false);
            return;
            break;
        case ErrorCodes::EmailNotExist:
            qDebug()<<"邮箱不存在";
            showTip(tr("邮箱不存在"),false);
            return;
            break;
        case ErrorCodes::RPC_FAILED:
            qDebug()<<"RPC调用失败";
            showTip(tr("RPC调用失败"),false);
            return;
            break;
        case ErrorCodes::SUCCESS:
            qDebug()<<"登录成功";
            showTip(tr("登录成功"),true);
        }

        ServerInfo server;
        server.host=jsonObj["host"].toString();
        server.port=jsonObj["port"].toString();
        server.token=jsonObj["token"].toString();
        qDebug()<<"token:"<<server.token;
        server.uid=jsonObj["uid"].toInt();
        qDebug()<<"uid:"<<server.uid;
        _uid=server.uid;
        _token=server.token;
        emit signal_tcp_connected(server);//连接服务器
        showTip(tr("正在连接服务器"),true);
    });
}

