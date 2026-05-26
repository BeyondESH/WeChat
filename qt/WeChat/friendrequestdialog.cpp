#include "friendrequestdialog.h"
#include <QLabel>

FriendRequestDialog::FriendRequestDialog(int fromUid, const QString &fromName,
                                       const QString &message, QWidget *parent)
    : QDialog(parent)
    , m_fromUid(fromUid)
    , m_fromName(fromName)
    , m_message(message)
    , m_accepted(false)
{
    setupUi();
    setWindowTitle(tr("Friend Request"));
    setFixedSize(400, 250);
}

FriendRequestDialog::~FriendRequestDialog() {}

void FriendRequestDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(tr("New Friend Request"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    QLabel *fromLabel = new QLabel(QString("%1 (ID: %2)").arg(m_fromName).arg(m_fromUid), this);
    fromLabel->setStyleSheet("font-size: 16px;");
    mainLayout->addWidget(fromLabel);

    QLabel *messageLabel = new QLabel(tr("Message:"), this);
    messageLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(messageLabel);

    QLabel *messageContentLabel = new QLabel(m_message, this);
    messageContentLabel->setWordWrap(true);
    mainLayout->addWidget(messageContentLabel);

    mainLayout->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *acceptBtn = new QPushButton(tr("Accept"), this);
    QPushButton *rejectBtn = new QPushButton(tr("Reject"), this);

    connect(acceptBtn, &QPushButton::clicked, this, &FriendRequestDialog::onAccept);
    connect(rejectBtn, &QPushButton::clicked, this, &FriendRequestDialog::onReject);

    btnLayout->addWidget(acceptBtn);
    btnLayout->addWidget(rejectBtn);
    mainLayout->addLayout(btnLayout);
}

void FriendRequestDialog::onAccept()
{
    m_accepted = true;
    accept();
}

void FriendRequestDialog::onReject()
{
    m_accepted = false;
    reject();
}
