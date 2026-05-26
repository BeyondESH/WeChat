#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    void reset();

signals:
    void loginSuccess(int uid, const QString &token, const QString &host, int port);

private slots:
    void onAccountLogin();
    void onEmailLogin();
    void onRegisterClicked();
    void onGetVerifyCode();

private:
    void setupUi();
    void setupConnections();

    QTabWidget *m_tabWidget;
    QLineEdit *m_accountInput;
    QLineEdit *m_passwordInput;
    QLineEdit *m_emailInput;
    QLineEdit *m_emailPasswordInput;
    QPushButton *m_loginBtn;
    QPushButton *m_registerBtn;
    QPushButton *m_getCodeBtn;
};

#endif // LOGINWIDGET_H
