#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"
#include <QTimer>
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    ,_isCorrect(true)
{
    ui->setupUi(this);
    polish();//刷新qss
    //处理注册完成信号
    connect(HttpMgr::getInstance().get(),&HttpMgr::signal_mod_register_finished,this,&RegisterDialog::slot_mod_register_finished);
    //初始化http请求处理函数
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    qDebug()<<"RegisterDialog调用了析构函数";
    delete ui;
}

void RegisterDialog::polish()
{
    ui->accountNotelabel->setProperty("state","normal");
    ui->passwordNotelabel->setProperty("state","normal");
    ui->verifyNodeLabel->setProperty("state","normal");
    ui->emailNotelabel->setProperty("state","normal");
    ui->codeNodeLabel->setProperty("state","normal");
    ui->errorlabel->setProperty("state","normal");
    repolish(ui->codeNodeLabel);
    repolish(ui->emailNotelabel);
    repolish(ui->verifyNodeLabel);
    repolish(ui->passwordNotelabel);
    repolish(ui->accountNotelabel);
    repolish(ui->errorlabel);
}

void RegisterDialog::showTip(QString tip, bool isOK)
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

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        ui->codeNodeLabel->setProperty("state","normal");
        repolish(ui->codeNodeLabel);
        ui->codeNodeLabel->setText("验证码已发送至邮箱，注意查收");

        //调试
        auto email=jsonObj["email"].toString();
        qDebug()<<"邮箱为："<<email;
    });

    //注册账号注册回包的逻辑
    _handlers.insert(ReqId::ID_REG_USER,[this](const QJsonObject &jsonObj){
        int error=jsonObj["error"].toInt();
        switch(error){
        case ErrorCodes::UserExist:
            showTip("用户已存在",false);
            return;
        case ErrorCodes::EmailExist:
            showTip("邮箱已存在",false);
            return;
        case ErrorCodes::ERR_NETWORK:
            showTip("网络连接错误",false);
            return;
        case ErrorCodes::VerifyCodeError:
            showTip("验证码错误",false);
            return;
        case ErrorCodes::VerifyCodeExpired:
            showTip("验证码过期或不存在",false);
            return;
        default:
            break;
        }
        showTip("用户注册成功",true);
        remainTime=6;
        QTimer *timer=new QTimer(this);
        connect(timer,&QTimer::timeout,[this,timer](){
            QString str=QString::number(remainTime)+"秒后返回登录界面";
            showTip(str,true);
            if(remainTime==0){
                timer->stop();
                timer->deleteLater();
                emit backPBClicked();
                return;
            }
            remainTime--;
        });
        timer->start(1000);
    });
}

void RegisterDialog::on_backPushButton_clicked()
{
    emit backPBClicked();
}


void RegisterDialog::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1==Qt::Checked){
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}


void RegisterDialog::on_checkBox_2_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1==Qt::Checked){
        ui->verifylineEdit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->verifylineEdit->setEchoMode(QLineEdit::Password);
    }
}


void RegisterDialog::on_getCodepushButton_clicked()
{
    ui->errorlabel->clear();
    ui->emailNotelabel->clear();
    ui->codeNodeLabel->clear();
    auto const email=ui->emailLineEdit->text();
    //判断邮箱是否为空
    if(email.isEmpty()){
        ui->codeNodeLabel->setProperty("state","error");
        repolish(ui->codeNodeLabel);
        ui->codeNodeLabel->setText("邮箱不能为空");
    }else{
        //判断邮箱格式是否正确
        QRegularExpression re("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");//正确的邮箱格式
        bool match=re.match(email).hasMatch();
        if(match){
            //发送验证码
            QJsonObject jsonObj;
            jsonObj["email"]=email;
            HttpMgr::getInstance()->postHttpRequest(QUrl(gate_url_prefix+"/get_varifycode"),jsonObj,ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
            //倒计时60s
            QTimer *updateTimer =new QTimer(this);
            remainTime=60;//60秒
            connect(updateTimer,&QTimer::timeout,[this,updateTimer](){
                if(remainTime>0){
                    ui->getCodepushButton->setEnabled(false);
                    ui->getCodepushButton->setText(QString::number(remainTime));
                    remainTime--;
                }else{
                    ui->getCodepushButton->setText("获取");
                    ui->getCodepushButton->setEnabled(true);
                    updateTimer->stop();
                    updateTimer->deleteLater();
                }
            });
            updateTimer->start(1000);//每秒更新一次
        }else{
            //格式不匹配
            ui->emailNotelabel->setProperty("state","error");
            repolish(ui->emailNotelabel);
            ui->emailNotelabel->setText("邮箱格式错误");
        }
    }
}

//处理注册完成信号的槽函数
void RegisterDialog::slot_mod_register_finished(ReqId req_id, QString res, ErrorCodes ec)
{
    //错误处理
    if(ec!=ErrorCodes::SUCCESS){
        if(req_id==ReqId::ID_GET_VARIFY_CODE){
            ui->codeNodeLabel->setProperty("state","error");
            repolish(ui->codeNodeLabel);
            ui->codeNodeLabel->setText("网路连接错误");
        }else if(req_id==ReqId::ID_REG_USER){
            showTip(tr("网络连接错误"),false);
        }
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

void RegisterDialog::on_submitPushButton_clicked()
{
    //qDebug()<<_isCorrect;
    ui->errorlabel->clear();
    if(ui->accountLineEdit->text()==nullptr){
        ui->accountNotelabel->setProperty("state","error");
        repolish(ui->accountNotelabel);
        ui->accountNotelabel->setText(tr("账号不能为空"));
        return;
    }
    if(ui->emailLineEdit->text()==nullptr){
        ui->emailNotelabel->setProperty("state","error");
        repolish(ui->emailNotelabel);
        ui->emailNotelabel->setText(tr("邮箱不能为空"));
        return;
    }
    if(ui->passwordLineEdit->text()==nullptr){
        ui->passwordNotelabel->setProperty("state","error");
        repolish(ui->passwordNotelabel);
        ui->passwordNotelabel->setText(tr("密码不能为空"));
        return;
    }
    if(ui->verifylineEdit->text()==nullptr){
        ui->verifyNodeLabel->setProperty("state","error");
        repolish(ui->verifyNodeLabel);
        ui->verifyNodeLabel->setText(tr("请再此确认"));
        return;
    }
    if(ui->verifylineEdit->text()!=ui->passwordLineEdit->text()){
        ui->verifyNodeLabel->setProperty("state","error");
        repolish(ui->verifyNodeLabel);
        ui->verifyNodeLabel->setText(tr("两次密码不相同"));
        return;
    }

    if(ui->codeLineEdit->text()==nullptr){
        ui->codeNodeLabel->setProperty("state","error");
        repolish(ui->codeNodeLabel);
        ui->codeNodeLabel->setText(tr("验证码不能为空"));
        return;
    }

    if(ui->codeLineEdit->text().length()!=6){
        ui->codeNodeLabel->setProperty("state","error");
        repolish(ui->codeNodeLabel);
        ui->codeNodeLabel->setText(tr("验证码错误"));
        return;
    }

    if(_isCorrect==false){
        return;
    }
    //发送注册请求
    QJsonObject infoJson;
    infoJson["user"]=ui->accountLineEdit->text();
    infoJson["password"]=stringToSha256(ui->passwordLineEdit->text());
    infoJson["email"]=ui->emailLineEdit->text();
    infoJson["verifyCode"]=ui->codeLineEdit->text();
    qDebug()<<infoJson["password"];
    HttpMgr::getInstance()->postHttpRequest(QUrl(gate_url_prefix+"/user_register"),infoJson,ReqId::ID_REG_USER,Modules::REGISTERMOD);
}


void RegisterDialog::on_accountLineEdit_textEdited(const QString &arg1)
{
    ui->accountNotelabel->clear();
    ui->accountNotelabel->setProperty("state","normal");
    repolish(ui->accountNotelabel);
    ui->accountNotelabel->setText(tr("账号由8~16个数字或字母组成"));
}


void RegisterDialog::on_accountLineEdit_editingFinished()
{
    _isCorrect=true;
    ui->accountNotelabel->clear();
    //判断账号格式是否正确
    if(ui->accountLineEdit->text()!=nullptr){
        QString account=ui->accountLineEdit->text();
        QRegularExpression re("^[a-zA-Z0-9]{8,16}$");//账号由8~16个数字或字母组成
        bool match=re.match(account).hasMatch();
        if(!match){
            //格式不匹配
            ui->accountNotelabel->setProperty("state","error");
            repolish(ui->accountNotelabel);
            ui->accountNotelabel->setText("账号格式错误");
            _isCorrect=false;
        }
    }
}


void RegisterDialog::on_emailLineEdit_editingFinished()
{
    _isCorrect=true;
    ui->emailNotelabel->clear();
    //判断邮箱是否为空
    auto email=ui->emailLineEdit->text();
    if(email==nullptr){
        ui->codeNodeLabel->setProperty("state","error");
        repolish(ui->codeNodeLabel);
        ui->codeNodeLabel->setText("邮箱不能为空");
    }else{
        //判断邮箱格式是否正确
        QRegularExpression re("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");//正确的邮箱格式
        bool match=re.match(email).hasMatch();
        if(!match){
            //格式不匹配
            ui->emailNotelabel->setProperty("state","error");
            repolish(ui->emailNotelabel);
            ui->emailNotelabel->setText("邮箱格式错误");
            _isCorrect=false;
        }
    }
}


void RegisterDialog::on_emailLineEdit_textEdited(const QString &arg1)
{

}


void RegisterDialog::on_passwordLineEdit_textEdited(const QString &arg1)
{
    ui->passwordNotelabel->clear();
    ui->passwordNotelabel->setProperty("state","normal");
    repolish(ui->passwordNotelabel);
    ui->passwordNotelabel->setText(tr("密码由8~20个数字或字母组成"));
}


void RegisterDialog::on_verifylineEdit_textEdited(const QString &arg1)
{
    ui->verifyNodeLabel->clear();
    ui->verifyNodeLabel->setProperty("state","normal");
    repolish(ui->passwordNotelabel);
    ui->verifyNodeLabel->setText(tr("请和密码保持相同"));
}


void RegisterDialog::on_verifylineEdit_editingFinished()
{
    _isCorrect=true;
    ui->verifyNodeLabel->clear();
    ui->verifyNodeLabel->setProperty("state","normal");
    repolish(ui->verifyNodeLabel);
    //比较是否密码相同
    if(ui->passwordLineEdit->text()!=ui->verifylineEdit->text()){
        ui->verifyNodeLabel->setProperty("state","error");
        repolish(ui->verifyNodeLabel);
        ui->verifyNodeLabel->setText(tr("两次密码不相同"));
        _isCorrect=false;
    }
}


void RegisterDialog::on_passwordLineEdit_editingFinished()
{
    _isCorrect=true;
    ui->passwordNotelabel->clear();
    ui->passwordNotelabel->setProperty("state","normal");
    repolish(ui->passwordNotelabel);
    QString password=ui->passwordLineEdit->text();
    if(password!=nullptr){
        //判断密码格式是否正确
        QRegularExpression re("^[A-Za-z0-9]{8,20}$");//密码由8~20个数字或字母组成
        bool match=re.match(password).hasMatch();
        if(!match){
            //格式不匹配
            ui->passwordNotelabel->setProperty("state","error");
            repolish(ui->passwordNotelabel);
            ui->passwordNotelabel->setText("密码格式错误");
            _isCorrect=false;
        }
    }
}


void RegisterDialog::on_codeLineEdit_editingFinished()
{
    ui->codeNodeLabel->clear();
    ui->codeNodeLabel->setProperty("state","normal");
    repolish(ui->codeNodeLabel);
}

