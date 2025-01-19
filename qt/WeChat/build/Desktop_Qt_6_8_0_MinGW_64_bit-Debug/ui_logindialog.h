/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *accountLabel;
    QLineEdit *accountLineEdit;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer_5;
    QLabel *accountNodeLabel;
    QHBoxLayout *horizontalLayout_2;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *registerPushButton;
    QSpacerItem *horizontalSpacer_7;
    QLabel *passwordNodeLabel;
    QSpacerItem *verticalSpacer_5;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_2;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_8;
    QSpacerItem *horizontalSpacer;
    QPushButton *loginButton;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QLabel *findPasswordLabel;
    QLabel *moreLabel;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(320, 400);
        QFont font;
        font.setFamilies({QString::fromUtf8("\351\273\221\344\275\223")});
        LoginDialog->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/loginDialog/img/wechat.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        LoginDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(LoginDialog);
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label = new QLabel(LoginDialog);
        label->setObjectName("label");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221 Light")});
        font1.setPointSize(15);
        font1.setBold(true);
        label->setFont(font1);
        label->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        label->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignHCenter);

        horizontalLayout_4->addWidget(label);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        accountLabel = new QLabel(LoginDialog);
        accountLabel->setObjectName("accountLabel");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\351\273\221\344\275\223")});
        font2.setPointSize(13);
        accountLabel->setFont(font2);
        accountLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        accountLabel->setIndent(0);

        horizontalLayout->addWidget(accountLabel);

        accountLineEdit = new QLineEdit(LoginDialog);
        accountLineEdit->setObjectName("accountLineEdit");
        accountLineEdit->setMinimumSize(QSize(0, 0));
        accountLineEdit->setMaximumSize(QSize(1000, 30));
        accountLineEdit->setBaseSize(QSize(0, 0));
        accountLineEdit->setFont(font2);
        accountLineEdit->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout->addWidget(accountLineEdit);

        horizontalSpacer_6 = new QSpacerItem(15, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 7);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");

        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        horizontalSpacer_5 = new QSpacerItem(52, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_5);

        accountNodeLabel = new QLabel(LoginDialog);
        accountNodeLabel->setObjectName("accountNodeLabel");
        accountNodeLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_12->addWidget(accountNodeLabel);


        verticalLayout->addLayout(horizontalLayout_12);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        passwordLabel = new QLabel(LoginDialog);
        passwordLabel->setObjectName("passwordLabel");
        passwordLabel->setFont(font2);
        passwordLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        passwordLabel->setIndent(0);

        horizontalLayout_2->addWidget(passwordLabel);

        passwordLineEdit = new QLineEdit(LoginDialog);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setMaximumSize(QSize(1000, 30));
        passwordLineEdit->setFont(font2);
        passwordLineEdit->setStyleSheet(QString::fromUtf8(""));
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

        horizontalLayout_2->addWidget(passwordLineEdit);

        checkBox = new QCheckBox(LoginDialog);
        checkBox->setObjectName("checkBox");
        checkBox->setChecked(false);

        horizontalLayout_2->addWidget(checkBox);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 7);

        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        registerPushButton = new QPushButton(LoginDialog);
        registerPushButton->setObjectName("registerPushButton");
        registerPushButton->setMaximumSize(QSize(35, 16777215));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\351\273\221\344\275\223")});
        font3.setPointSize(10);
        registerPushButton->setFont(font3);
        registerPushButton->setStyleSheet(QString::fromUtf8("color: rgb(97, 101, 154);"));

        horizontalLayout_11->addWidget(registerPushButton);

        horizontalSpacer_7 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_7);

        passwordNodeLabel = new QLabel(LoginDialog);
        passwordNodeLabel->setObjectName("passwordNodeLabel");
        passwordNodeLabel->setStyleSheet(QString::fromUtf8(""));
        passwordNodeLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_11->addWidget(passwordNodeLabel);


        verticalLayout->addLayout(horizontalLayout_11);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        label_2 = new QLabel(LoginDialog);
        label_2->setObjectName("label_2");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setMinimumSize(QSize(0, 25));
        label_2->setMaximumSize(QSize(16777215, 25));
        label_2->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        label_2->setStyleSheet(QString::fromUtf8("color: rgb(83, 83, 83);"));
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_6->addWidget(label_2);


        verticalLayout->addLayout(horizontalLayout_6);

        verticalSpacer_4 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer_4);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer);

        loginButton = new QPushButton(LoginDialog);
        loginButton->setObjectName("loginButton");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(loginButton->sizePolicy().hasHeightForWidth());
        loginButton->setSizePolicy(sizePolicy1);
        loginButton->setMinimumSize(QSize(105, 35));
        loginButton->setMaximumSize(QSize(16777215, 16777215));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\351\273\221\344\275\223")});
        font4.setPointSize(12);
        font4.setWeight(QFont::DemiBold);
        loginButton->setFont(font4);
        loginButton->setStyleSheet(QString::fromUtf8("background-color: rgb(5, 193, 96);\n"
"color: rgb(255, 255, 255);\n"
"\n"
""));

        horizontalLayout_8->addWidget(loginButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(10);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        findPasswordLabel = new QLabel(LoginDialog);
        findPasswordLabel->setObjectName("findPasswordLabel");
        findPasswordLabel->setFont(font);
        findPasswordLabel->setStyleSheet(QString::fromUtf8("color: rgb(97, 101, 154);"));
        findPasswordLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_3->addWidget(findPasswordLabel);

        moreLabel = new QLabel(LoginDialog);
        moreLabel->setObjectName("moreLabel");
        moreLabel->setFont(font);
        moreLabel->setStyleSheet(QString::fromUtf8("color: rgb(97, 101, 154);"));
        moreLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_3->addWidget(moreLabel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer_3 = new QSpacerItem(20, 15, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer_3);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "\347\231\273\345\275\225", nullptr));
        label->setText(QCoreApplication::translate("LoginDialog", "\345\276\256\344\277\241\345\217\267/\351\202\256\347\256\261\347\231\273\351\231\206", nullptr));
        accountLabel->setText(QCoreApplication::translate("LoginDialog", "\350\264\246\345\217\267", nullptr));
        accountLineEdit->setText(QString());
        accountLineEdit->setPlaceholderText(QCoreApplication::translate("LoginDialog", "\345\276\256\344\277\241\345\217\267/\351\202\256\347\256\261", nullptr));
        accountNodeLabel->setText(QString());
        passwordLabel->setText(QCoreApplication::translate("LoginDialog", "\345\257\206\347\240\201", nullptr));
        passwordLineEdit->setText(QString());
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("LoginDialog", "\350\257\267\345\241\253\345\206\231\345\257\206\347\240\201", nullptr));
        checkBox->setText(QString());
        registerPushButton->setText(QCoreApplication::translate("LoginDialog", "\346\263\250\345\206\214", nullptr));
        passwordNodeLabel->setText(QString());
        label_2->setText(QCoreApplication::translate("LoginDialog", "\344\270\212\350\257\211\345\276\256\344\277\241\345\217\267\344\273\205\347\224\250\344\272\216\347\231\273\345\275\225\351\252\214\350\257\201", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "\345\220\214\346\204\217\345\271\266\347\231\273\345\275\225", nullptr));
        findPasswordLabel->setText(QCoreApplication::translate("LoginDialog", "\346\211\276\345\233\236\345\257\206\347\240\201", nullptr));
        moreLabel->setText(QCoreApplication::translate("LoginDialog", "\346\233\264\345\244\232\351\200\211\351\241\271", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
