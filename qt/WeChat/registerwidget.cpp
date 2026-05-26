#include "registerwidget.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "httpclient.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , m_emailInput(new QLineEdit(this))
    , m_verifyCodeInput(new QLineEdit(this))
    , m_getCodeBtn(new QPushButton(tr("Get Code"), this))
    , m_userNameInput(new QLineEdit(this))
    , m_passwordInput(new QLineEdit(this))
    , m_confirmPasswordInput(new QLineEdit(this))
    , m_registerBtn(new QPushButton(tr("Register"), this))
    , m_cancelBtn(new QPushButton(tr("Cancel"), this))
{
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_confirmPasswordInput->setEchoMode(QLineEdit::Password);

    setupUi();
    setupConnections();

    setWindowTitle(tr("Register"));
    setFixedSize(400, 400);
}

RegisterDialog::~RegisterDialog() {}

void RegisterDialog::setupUi()
{
    QFormLayout *formLayout = new QFormLayout(this);

    formLayout->addRow(tr("Email:"), m_emailInput);

    QHBoxLayout *codeLayout = new QHBoxLayout();
    codeLayout->addWidget(m_verifyCodeInput);
    codeLayout->addWidget(m_getCodeBtn);
    formLayout->addRow(tr("Verify Code:"), codeLayout);

    formLayout->addRow(tr("User Name:"), m_userNameInput);
    formLayout->addRow(tr("Password:"), m_passwordInput);
    formLayout->addRow(tr("Confirm:"), m_confirmPasswordInput);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_cancelBtn);
    formLayout->addRow(btnLayout);
}

void RegisterDialog::setupConnections()
{
    connect(m_getCodeBtn, &QPushButton::clicked, this, &RegisterDialog::onGetVerifyCode);
    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterDialog::onRegister);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void RegisterDialog::onGetVerifyCode()
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

void RegisterDialog::onRegister()
{
    QString email = m_emailInput->text();
    QString verifyCode = m_verifyCodeInput->text();
    QString userName = m_userNameInput->text();
    QString password = m_passwordInput->text();
    QString confirmPassword = m_confirmPasswordInput->text();

    if (email.isEmpty() || verifyCode.isEmpty() || userName.isEmpty() ||
        password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill in all fields!"));
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, tr("Error"), tr("Passwords do not match!"));
        return;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, tr("Error"), tr("Password must be at least 6 characters!"));
        return;
    }

    HttpClient::instance()->registerUser(email, verifyCode, userName, password,
        [this](bool success, const QString &error) {
        if (success) {
            QMessageBox::information(this, tr("Success"),
                                   tr("Registration successful!"));
            accept();
        } else {
            QMessageBox::warning(this, tr("Error"),
                               tr("Registration failed: %1").arg(error));
        }
    });
}
