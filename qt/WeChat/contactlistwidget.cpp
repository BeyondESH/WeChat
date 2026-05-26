#include "contactlistwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include "httpclient.h"
#include "tcpclient.h"
#include "searchuserdialog.h"

ContactListWidget::ContactListWidget(QWidget *parent)
    : QWidget(parent)
    , m_contactList(new QListWidget(this))
    , m_addFriendBtn(new QPushButton(tr("Add Friend"), this))
    , m_refreshBtn(new QPushButton(tr("Refresh"), this))
{
    setupUi();
}

ContactListWidget::~ContactListWidget() {}

void ContactListWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel(tr("Contacts"), this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; padding: 15px;");
    mainLayout->addWidget(title);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_addFriendBtn);
    btnLayout->addWidget(m_refreshBtn);
    mainLayout->addLayout(btnLayout);

    mainLayout->addWidget(m_contactList);

    m_contactList->setSpacing(5);
    m_contactList->setItemAlignment(Qt::AlignHCenter);

    connect(m_contactList, &QListWidget::itemDoubleClicked,
            this, &ContactListWidget::onContactClicked);
    connect(m_addFriendBtn, &QPushButton::clicked,
            this, &ContactListWidget::onAddFriendClicked);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &ContactListWidget::onRefreshClicked);
}

void ContactListWidget::updateContactList(const QVector<UserInfo> &contacts)
{
    m_contactList->clear();
    m_contacts.clear();

    for (const UserInfo &contact : contacts) {
        m_contacts[contact.uid] = contact;

        QListWidgetItem *item = new QListWidgetItem();
        QString displayText = QString("%1 %2")
            .arg(contact.name)
            .arg(contact.isOnline ? "[Online]" : "[Offline]");

        if (contact.isOnline) {
            displayText = QString("<font color='green'>%1</font> [Online]").arg(contact.name);
        } else {
            displayText = QString("<font color='gray'>%1</font> [Offline]").arg(contact.name);
        }

        item->setData(Qt::UserRole, contact.uid);
        m_contactList->addItem(displayText);
    }
}

void ContactListWidget::onContactClicked(QListWidgetItem *item)
{
    int uid = item->data(Qt::UserRole).toInt();
    if (m_contacts.contains(uid)) {
        QString name = m_contacts[uid].name;
        emit openChat(uid, name);
    }
}

void ContactListWidget::onAddFriendClicked()
{
    SearchUserDialog dialog(this);
    dialog.exec();
}

void ContactListWidget::onRefreshClicked()
{
    int uid = UserManager::instance()->getCurrentUid();
    QString token = UserManager::instance()->getCurrentToken();

    HttpClient::instance()->getFriendList(uid, token, [this](const QJsonArray &friends) {
        QVector<UserInfo> userList;
        for (const auto &friendJson : friends) {
            UserInfo info;
            info.uid = friendJson["uid"].toInt();
            info.name = friendJson["name"].toString();
            info.avatar = friendJson["avatar"].toString();
            info.status = friendJson["status"].toString();
            info.isOnline = friendJson["isOnline"].toBool();
            userList.append(info);
        }
        updateContactList(userList);
    });
}
