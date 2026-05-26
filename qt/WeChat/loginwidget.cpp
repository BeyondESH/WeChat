#include "loginwidget.h"
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QTimer>
#include "httpclient.h"
#include "usermanager.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , m_tabWidget(new QTabWidget(this))
    , m_accountInput(new QLineEdit(this))
    , m_passwordInput(new QLineEdit(this))
    , m_emailInput(new QLineEdit(this))
    , m_emailPasswordInput(new QLineEdit(this))
    , m_loginBtn(new QPushButton(tr("Login"), this))
    , m_registerBtn(new QPushButton(tr("Register"), this))
    , m_getCodeBtn(new QPushButton(tr("Get Code"), this))
{
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_emailPasswordInput->setEchoMode(QLineEdit::Password);

    setupUi();
    setupConnections();
}

LoginWidget::~LoginWidget() {}

void LoginWidget::setupUi()
{
    setFixedSize(400, 500);
    setWindowTitle(tr("Login"));

    QWidget *accountPage = new QWidget(this);
    QFormLayout *accountForm = new QFormLayout(accountPage);
    accountForm->addRow(tr("Account:"), m_accountInput);
    accountForm->addRow(tr("Password:"), m_passwordInput);

    QVBoxLayout *accountBtnLayout = new QVBoxLayout();
    accountBtnLayout->addWidget(m_loginBtn);
    accountBtnLayout->addWidget(m_registerBtn);
    accountForm->addRow(accountBtnLayout);

    QWidget *emailPage = new QWidget(this);
    QFormLayout *emailForm = new QFormLayout(emailPage);
    emailForm->addRow(tr("Email:"), m_emailInput);
    emailForm->addRow(tr("Password:"), m_emailPasswordInput);

    QHBoxLayout *codeLayout = new QHBoxLayout();
    codeLayout->addWidget(m_getCodeBtn);
    emailForm->addRow(tr("Code:"), codeLayout);

    QPushButton *emailLoginBtn = new QPushButton(tr("Login"), this);
    QPushButton *emailResetBtn = new QPushButton(tr("Reset Password"), this);
    QVBoxLayout *emailBtnLayout = new QVBoxLayout();
    emailBtnLayout->addWidget(emailLoginBtn);
    emailBtnLayout->addWidget(emailResetBtn);
    emailForm->addRow(emailBtnLayout);

    connect(emailLoginBtn, &QPushButton::clicked, this, &LoginWidget::onEmailLogin);
    connect(emailResetBtn, &QPushButton::clicked, this, [this]() {
        QString email = m_emailInput->text();
        QString code = QInputDialog::getText(this, tr("Reset Password"),
                                           tr("Enter verification code:"));
        QString newPassword = QInputDialog::getText(this, tr("Reset Password"),
                                                   tr("Enter new password:"),
                                                   QLineEdit::Password);
        if (!email.isEmpty() && !code.isEmpty() && !newPassword.isEmpty()) {
            HttpClient::instance()->resetPassword(email, code, newPassword,
                                                 [this](bool success) {
                if (success) {
                    QMessageBox::information(this, tr("Success"),
                                           tr("Password reset successfully!"));
                } else {
                    QMessageBox::warning(this, tr("Error"),
                                        tr("Failed to reset password!"));
                }
            });
        }
    });

    m_tabWidget->addTab(accountPage, tr("Account Login"));
    m_tabWidget->addTab(emailPage, tr("Email Login"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *title = new QLabel(tr("Distributed Chat System"), this);
    title->setAlignment(Qt::AlignCenter);
    QFont font = title->font();
    font.setPointSize(20);
    font.setBold(true);
    title->setFont(font);

    mainLayout->addWidget(title);
    mainLayout->addWidget(m_tabWidget);
}

void LoginWidget::setupConnections()
{
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWidget::onAccountLogin);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginWidget::onRegisterClicked);
    connect(m_getCodeBtn, &QPushButton::clicked, this, &LoginWidget::onGetVerifyCode);
}

void LoginWidget::onAccountLogin()
{
    QString account = m_accountInput->text();
    QString password = m_passwordInput->text();

    if (account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill in all fields!"));
        return;
    }

    m_loginBtn->setEnabled(false);
    m_loginBtn->setText(tr("Logging in..."));

    HttpClient::instance()->loginByAccount(account, password,
        [this](bool success, int uid, const QString &token,
               const QString &host, int port, const QString &userName) {
        m_loginBtn->setEnabled(true);
        m_loginBtn->setText(tr("Login"));

        if (success) {
            UserManager::instance()->setCurrentUser(uid, userName, "", "", token);
            emit loginSuccess(uid, token, host, port);
        } else {
            QMessageBox::warning(this, tr("Login Failed"),
                               tr("Invalid account or password!"));
        }
    });
}

void LoginWidget::onEmailLogin()
{
    QString email = m_emailInput->text();
    QString password = m_emailPasswordInput->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill in all fields!"));
        return;
    }

    HttpClient::instance()->loginByEmail(email, password,
        [this](bool success, int uid, const QString &token,
               const QString &host, int port, const QString &userName) {
        if (success) {
            UserManager::instance()->setCurrentUser(uid, userName, email, "", token);
            emit loginSuccess(uid, token, host, port);
        } else {
            QMessageBox::warning(this, tr("Login Failed"),
                               tr("Invalid email or password!"));
        }
    });
}

void LoginWidget::onRegisterClicked()
{
    RegisterDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, tr("Success"),
                               tr("Registration successful! Please login."));
    }
}

void LoginWidget::onGetVerifyCode()
{
    QString email = m_emailInput->text();

    if (email.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please enter your email!"));
        return;
    }

    m_getCodeBtn->setEnabled(false);
    int countdown = 60;
    m_getCodeBtn->setText(QString::number(countdown));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, &countdown, timer]() {
        countdown--;
        if (countdown > 0) {
            m_getCodeBtn->setText(QString::number(countdown));
        } else {
            timer->stop();
            m_getCodeBtn->setEnabled(true);
            m_getCodeBtn->setText(tr("Get Code"));
            delete timer;
        }
    });
    timer->start(1000);

    HttpClient::instance()->getVerifyCode(email, [this](bool success) {
        if (success) {
            QMessageBox::information(this, tr("Success"),
                                   tr("Verification code sent!"));
        } else {
            QMessageBox::warning(this, tr("Error"),
                               tr("Failed to send code!"));
        }
    });
}

void LoginWidget::reset()
{
    m_accountInput->clear();
    m_passwordInput->clear();
    m_emailInput->clear();
    m_emailPasswordInput->clear();
    m_tabWidget->setCurrentIndex(0);
}
