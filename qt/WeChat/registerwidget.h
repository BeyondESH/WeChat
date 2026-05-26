#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void onGetVerifyCode();
    void onRegister();

private:
    void setupUi();
    void setupConnections();

    QLineEdit *m_emailInput;
    QLineEdit *m_verifyCodeInput;
    QPushButton *m_getCodeBtn;
    QLineEdit *m_userNameInput;
    QLineEdit *m_passwordInput;
    QLineEdit *m_confirmPasswordInput;
    QPushButton *m_registerBtn;
    QPushButton *m_cancelBtn;
};

#endif // REGISTERWIDGET_H
