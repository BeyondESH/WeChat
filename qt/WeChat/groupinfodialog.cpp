#include "groupinfodialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include "httpclient.h"
#include "tcpclient.h"
#include "usermanager.h"

GroupInfoDialog::GroupInfoDialog(int groupId, QWidget *parent)
    : QDialog(parent)
    , m_groupId(groupId)
    , m_memberList(new QListWidget(this))
    , m_addMemberBtn(new QPushButton(tr("Add Member"), this))
    , m_removeMemberBtn(new QPushButton(tr("Remove Member"), this))
    , m_closeBtn(new QPushButton(tr("Close"), this))
{
    setupUi();
    loadGroupMembers();
    setWindowTitle(tr("Group Info"));
    setFixedSize(400, 500);
}

GroupInfoDialog::~GroupInfoDialog() {}

void GroupInfoDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(tr("Group Members"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    mainLayout->addWidget(m_memberList);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_addMemberBtn);
    btnLayout->addWidget(m_removeMemberBtn);
    btnLayout->addWidget(m_closeBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_addMemberBtn, &QPushButton::clicked, this, &GroupInfoDialog::onAddMemberClicked);
    connect(m_removeMemberBtn, &QPushButton::clicked, this, &GroupInfoDialog::onRemoveMemberClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void GroupInfoDialog::onAddMemberClicked()
{
    bool ok;
    int memberUid = QInputDialog::getInt(this, tr("Add Member"),
                                       tr("Enter user ID:"),
                                       0, 0, 999999, 1, &ok);
    if (!ok) {
        return;
    }

    QString token = UserManager::instance()->getCurrentToken();
    HttpClient::instance()->addGroupMember(m_groupId, memberUid, token,
        [this](bool success) {
        if (success) {
            QMessageBox::information(this, tr("Success"),
                                   tr("Member added successfully!"));
            loadGroupMembers();
        } else {
            QMessageBox::warning(this, tr("Error"),
                              tr("Failed to add member!"));
        }
    });
}

void GroupInfoDialog::onRemoveMemberClicked()
{
    QListWidgetItem *currentItem = m_memberList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a member!"));
        return;
    }

    int memberUid = currentItem->data(Qt::UserRole).toInt();
    QString memberName = currentItem->text();

    int currentUid = UserManager::instance()->getCurrentUid();
    if (memberUid == currentUid) {
        QMessageBox::warning(this, tr("Error"),
                          tr("You cannot remove yourself!"));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm"),
                                QString("Remove %1 from the group?").arg(memberName),
                                QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        TcpClient::instance()->sendRemoveGroupMember(m_groupId, memberUid);
        loadGroupMembers();
    }
}

void GroupInfoDialog::loadGroupMembers()
{
    QString token = UserManager::instance()->getCurrentToken();
    HttpClient::instance()->getGroupMembers(m_groupId, token,
        [this](const QJsonArray &members) {
        m_memberList->clear();
        for (const auto &memberValue : members) {
            QJsonObject member = memberValue.toObject();
            QString name = member["name"].toString();
            int uid = member["uid"].toInt();
            QString role = member["role"].toString();

            QListWidgetItem *item = new QListWidgetItem();
            QString displayText = QString("%1 (ID: %2) - %3")
                .arg(name)
                .arg(uid)
                .arg(role);
            item->setData(Qt::UserRole, uid);
            m_memberList->addItem(displayText);
        }
    });
}
