/********************************************************************************
** Form generated from reading UI file 'registerdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERDIALOG_H
#define UI_REGISTERDIALOG_H

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
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *accountHL;
    QSpacerItem *horizontalSpacer_11;
    QLabel *accountLabel;
    QLineEdit *accountLineEdit;
    QSpacerItem *horizontalSpacer_12;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_3;
    QLabel *accountNotelabel;
    QHBoxLayout *eamilHL;
    QSpacerItem *horizontalSpacer_10;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QSpacerItem *horizontalSpacer_13;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QLabel *emailNotelabel;
    QHBoxLayout *passwordHL;
    QSpacerItem *horizontalSpacer_9;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QCheckBox *checkBox;
    QSpacerItem *horizontalSpacer_15;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_5;
    QLabel *passwordNotelabel;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_8;
    QLabel *verifyLabel;
    QLineEdit *verifylineEdit;
    QCheckBox *checkBox_2;
    QSpacerItem *horizontalSpacer_14;
    QHBoxLayout *horizontalLayout_8;
    QSpacerItem *horizontalSpacer_6;
    QLabel *verifyNodeLabel;
    QHBoxLayout *horizontalLayout_3;
    QLabel *codeLabel;
    QLineEdit *codeLineEdit;
    QPushButton *getCodepushButton;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_7;
    QLabel *codeNodeLabel;
    QHBoxLayout *horizontalLayout_13;
    QSpacerItem *horizontalSpacer_16;
    QLabel *errorlabel;
    QSpacerItem *horizontalSpacer_17;
    QSpacerItem *verticalSpacer_5;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *submitPushButton;
    QPushButton *backPushButton;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QWidget *page_2;

    void setupUi(QDialog *RegisterDialog)
    {
        if (RegisterDialog->objectName().isEmpty())
            RegisterDialog->setObjectName("RegisterDialog");
        RegisterDialog->resize(350, 500);
        RegisterDialog->setMinimumSize(QSize(350, 500));
        RegisterDialog->setMaximumSize(QSize(16777215, 16777215));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/loginDialog/img/wechat.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        RegisterDialog->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(RegisterDialog);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        stackedWidget = new QStackedWidget(RegisterDialog);
        stackedWidget->setObjectName("stackedWidget");
        page = new QWidget();
        page->setObjectName("page");
        verticalLayout_3 = new QVBoxLayout(page);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_2 = new QLabel(page);
        label_2->setObjectName("label_2");
        label_2->setMaximumSize(QSize(16777215, 25));
        QFont font;
        font.setPointSize(15);
        font.setBold(true);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(label_2);


        verticalLayout_3->addLayout(horizontalLayout);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);

        accountHL = new QHBoxLayout();
        accountHL->setObjectName("accountHL");
        horizontalSpacer_11 = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        accountHL->addItem(horizontalSpacer_11);

        accountLabel = new QLabel(page);
        accountLabel->setObjectName("accountLabel");
        accountLabel->setMinimumSize(QSize(0, 25));
        accountLabel->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setPointSize(13);
        accountLabel->setFont(font1);

        accountHL->addWidget(accountLabel);

        accountLineEdit = new QLineEdit(page);
        accountLineEdit->setObjectName("accountLineEdit");
        accountLineEdit->setMinimumSize(QSize(0, 25));
        accountLineEdit->setMaximumSize(QSize(16777215, 25));
        accountLineEdit->setFont(font1);

        accountHL->addWidget(accountLineEdit);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        accountHL->addItem(horizontalSpacer_12);


        verticalLayout_3->addLayout(accountHL);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(-1, 0, -1, -1);
        horizontalSpacer_3 = new QSpacerItem(70, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        accountNotelabel = new QLabel(page);
        accountNotelabel->setObjectName("accountNotelabel");
        accountNotelabel->setMinimumSize(QSize(0, 13));
        accountNotelabel->setMaximumSize(QSize(16777215, 13));
        QFont font2;
        font2.setPointSize(10);
        accountNotelabel->setFont(font2);
        accountNotelabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_5->addWidget(accountNotelabel);


        verticalLayout_3->addLayout(horizontalLayout_5);

        eamilHL = new QHBoxLayout();
        eamilHL->setObjectName("eamilHL");
        horizontalSpacer_10 = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        eamilHL->addItem(horizontalSpacer_10);

        emailLabel = new QLabel(page);
        emailLabel->setObjectName("emailLabel");
        emailLabel->setMinimumSize(QSize(0, 25));
        emailLabel->setMaximumSize(QSize(16777215, 25));
        emailLabel->setFont(font1);

        eamilHL->addWidget(emailLabel);

        emailLineEdit = new QLineEdit(page);
        emailLineEdit->setObjectName("emailLineEdit");
        emailLineEdit->setMinimumSize(QSize(0, 25));
        emailLineEdit->setMaximumSize(QSize(16777215, 25));
        emailLineEdit->setFont(font1);

        eamilHL->addWidget(emailLineEdit);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        eamilHL->addItem(horizontalSpacer_13);


        verticalLayout_3->addLayout(eamilHL);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalSpacer_4 = new QSpacerItem(70, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        emailNotelabel = new QLabel(page);
        emailNotelabel->setObjectName("emailNotelabel");
        emailNotelabel->setMinimumSize(QSize(0, 13));
        emailNotelabel->setMaximumSize(QSize(16777215, 13));
        emailNotelabel->setFont(font2);
        emailNotelabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_6->addWidget(emailNotelabel);


        verticalLayout_3->addLayout(horizontalLayout_6);

        passwordHL = new QHBoxLayout();
        passwordHL->setObjectName("passwordHL");
        horizontalSpacer_9 = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        passwordHL->addItem(horizontalSpacer_9);

        passwordLabel = new QLabel(page);
        passwordLabel->setObjectName("passwordLabel");
        passwordLabel->setMinimumSize(QSize(0, 25));
        passwordLabel->setMaximumSize(QSize(16777215, 25));
        passwordLabel->setFont(font1);

        passwordHL->addWidget(passwordLabel);

        passwordLineEdit = new QLineEdit(page);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setMinimumSize(QSize(0, 25));
        passwordLineEdit->setMaximumSize(QSize(16777215, 25));
        passwordLineEdit->setFont(font1);
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

        passwordHL->addWidget(passwordLineEdit);

        checkBox = new QCheckBox(page);
        checkBox->setObjectName("checkBox");

        passwordHL->addWidget(checkBox);

        horizontalSpacer_15 = new QSpacerItem(20, 10, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        passwordHL->addItem(horizontalSpacer_15);


        verticalLayout_3->addLayout(passwordHL);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalSpacer_5 = new QSpacerItem(70, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_5);

        passwordNotelabel = new QLabel(page);
        passwordNotelabel->setObjectName("passwordNotelabel");
        passwordNotelabel->setMinimumSize(QSize(0, 13));
        passwordNotelabel->setMaximumSize(QSize(16777215, 13));
        passwordNotelabel->setFont(font2);
        passwordNotelabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_7->addWidget(passwordNotelabel);


        verticalLayout_3->addLayout(horizontalLayout_7);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer_8 = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_8);

        verifyLabel = new QLabel(page);
        verifyLabel->setObjectName("verifyLabel");
        verifyLabel->setMinimumSize(QSize(0, 25));
        verifyLabel->setMaximumSize(QSize(16777215, 25));
        verifyLabel->setFont(font1);

        horizontalLayout_4->addWidget(verifyLabel);

        verifylineEdit = new QLineEdit(page);
        verifylineEdit->setObjectName("verifylineEdit");
        verifylineEdit->setMinimumSize(QSize(0, 25));
        verifylineEdit->setMaximumSize(QSize(16777215, 25));
        verifylineEdit->setFont(font1);
        verifylineEdit->setEchoMode(QLineEdit::EchoMode::Password);

        horizontalLayout_4->addWidget(verifylineEdit);

        checkBox_2 = new QCheckBox(page);
        checkBox_2->setObjectName("checkBox_2");

        horizontalLayout_4->addWidget(checkBox_2);

        horizontalSpacer_14 = new QSpacerItem(20, 10, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_14);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalSpacer_6 = new QSpacerItem(70, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_6);

        verifyNodeLabel = new QLabel(page);
        verifyNodeLabel->setObjectName("verifyNodeLabel");
        verifyNodeLabel->setMinimumSize(QSize(0, 13));
        verifyNodeLabel->setMaximumSize(QSize(16777215, 13));
        verifyNodeLabel->setFont(font2);
        verifyNodeLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_8->addWidget(verifyNodeLabel);


        verticalLayout_3->addLayout(horizontalLayout_8);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        codeLabel = new QLabel(page);
        codeLabel->setObjectName("codeLabel");
        codeLabel->setFont(font1);

        horizontalLayout_3->addWidget(codeLabel);

        codeLineEdit = new QLineEdit(page);
        codeLineEdit->setObjectName("codeLineEdit");
        codeLineEdit->setFont(font1);

        horizontalLayout_3->addWidget(codeLineEdit);

        getCodepushButton = new QPushButton(page);
        getCodepushButton->setObjectName("getCodepushButton");
        getCodepushButton->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_3->addWidget(getCodepushButton);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalSpacer_7 = new QSpacerItem(70, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_7);

        codeNodeLabel = new QLabel(page);
        codeNodeLabel->setObjectName("codeNodeLabel");
        codeNodeLabel->setMinimumSize(QSize(0, 13));
        codeNodeLabel->setMaximumSize(QSize(16777215, 13));
        codeNodeLabel->setFont(font2);
        codeNodeLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_10->addWidget(codeNodeLabel);


        verticalLayout_3->addLayout(horizontalLayout_10);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        horizontalSpacer_16 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_16);

        errorlabel = new QLabel(page);
        errorlabel->setObjectName("errorlabel");

        horizontalLayout_13->addWidget(errorlabel);

        horizontalSpacer_17 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_17);


        verticalLayout_3->addLayout(horizontalLayout_13);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_5);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        submitPushButton = new QPushButton(page);
        submitPushButton->setObjectName("submitPushButton");
        submitPushButton->setMinimumSize(QSize(100, 35));
        submitPushButton->setMaximumSize(QSize(100, 35));
        QFont font3;
        font3.setPointSize(13);
        font3.setBold(true);
        submitPushButton->setFont(font3);
        submitPushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(5, 193, 96);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout_2->addWidget(submitPushButton);

        backPushButton = new QPushButton(page);
        backPushButton->setObjectName("backPushButton");
        backPushButton->setMinimumSize(QSize(100, 35));
        backPushButton->setMaximumSize(QSize(100, 35));
        backPushButton->setFont(font3);

        horizontalLayout_2->addWidget(backPushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_2);


        verticalLayout_3->addLayout(verticalLayout_5);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        stackedWidget->addWidget(page_2);

        verticalLayout->addWidget(stackedWidget);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(RegisterDialog);

        QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
        RegisterDialog->setWindowTitle(QCoreApplication::translate("RegisterDialog", "\346\263\250\345\206\214", nullptr));
        label_2->setText(QCoreApplication::translate("RegisterDialog", "\350\264\246\345\217\267\346\263\250\345\206\214", nullptr));
        accountLabel->setText(QCoreApplication::translate("RegisterDialog", "\350\264\246\345\217\267", nullptr));
        accountLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\350\257\267\350\276\223\345\205\245\350\264\246\345\217\267", nullptr));
        accountNotelabel->setText(QString());
        emailLabel->setText(QCoreApplication::translate("RegisterDialog", "\351\202\256\347\256\261", nullptr));
        emailLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\350\257\267\350\276\223\345\205\245\344\273\245\350\216\267\345\217\226\351\252\214\350\257\201\347\240\201", nullptr));
        emailNotelabel->setText(QString());
        passwordLabel->setText(QCoreApplication::translate("RegisterDialog", "\345\257\206\347\240\201", nullptr));
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        checkBox->setText(QString());
        passwordNotelabel->setText(QString());
        verifyLabel->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244", nullptr));
        verifylineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\350\257\267\345\206\215\346\254\241\347\241\256\350\256\244\345\257\206\347\240\201", nullptr));
        checkBox_2->setText(QString());
        verifyNodeLabel->setText(QString());
        codeLabel->setText(QCoreApplication::translate("RegisterDialog", "\351\252\214\350\257\201\347\240\201", nullptr));
        codeLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\350\257\267\350\276\223\345\205\245\351\252\214\350\257\201\347\240\201", nullptr));
        getCodepushButton->setText(QCoreApplication::translate("RegisterDialog", "\350\216\267\345\217\226", nullptr));
        codeNodeLabel->setText(QString());
        errorlabel->setText(QString());
        submitPushButton->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244", nullptr));
        backPushButton->setText(QCoreApplication::translate("RegisterDialog", "\350\277\224\345\233\236", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERDIALOG_H
