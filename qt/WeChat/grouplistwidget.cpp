#include "grouplistwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include "groupinfodialog.h"

GroupListWidget::GroupListWidget(QWidget *parent)
    : QWidget(parent)
    , m_groupList(new QListWidget(this))
    , m_groupInfoBtn(new QPushButton(tr("Group Info"), this))
    , m_refreshBtn(new QPushButton(tr("Refresh"), this))
{
    setupUi();
}

GroupListWidget::~GroupListWidget() {}

void GroupListWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel(tr("Groups"), this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; padding: 15px;");
    mainLayout->addWidget(title);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_groupInfoBtn);
    btnLayout->addWidget(m_refreshBtn);
    mainLayout->addLayout(btnLayout);

    mainLayout->addWidget(m_groupList);

    m_groupList->setSpacing(5);

    connect(m_groupList, &QListWidget::itemDoubleClicked,
            this, &GroupListWidget::onGroupClicked);
    connect(m_groupInfoBtn, &QPushButton::clicked,
            this, &GroupListWidget::onGroupInfoClicked);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &GroupListWidget::onRefreshClicked);
}

void GroupListWidget::updateGroupList(const QVector<GroupInfo> &groups)
{
    m_groupList->clear();
    m_groups.clear();

    for (const GroupInfo &group : groups) {
        m_groups[group.groupId] = group;

        QListWidgetItem *item = new QListWidgetItem();
        QString displayText = QString("%1 (%2 members)")
            .arg(group.name)
            .arg(group.memberCount);

        item->setData(Qt::UserRole, group.groupId);
        m_groupList->addItem(displayText);
    }
}

void GroupListWidget::onGroupClicked(QListWidgetItem *item)
{
    int groupId = item->data(Qt::UserRole).toInt();
    if (m_groups.contains(groupId)) {
        QString name = m_groups[groupId].name;
        emit openGroupChat(groupId, name);
    }
}

void GroupListWidget::onGroupInfoClicked()
{
    QListWidgetItem *currentItem = m_groupList->currentItem();
    if (!currentItem) {
        return;
    }

    int groupId = currentItem->data(Qt::UserRole).toInt();
    if (m_groups.contains(groupId)) {
        GroupInfoDialog dialog(groupId, this);
        dialog.exec();
    }
}

void GroupListWidget::onRefreshClicked()
{
    emit refreshRequested();
}
