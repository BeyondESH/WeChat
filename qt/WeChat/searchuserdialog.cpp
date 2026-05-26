#include "searchuserdialog.h"
#include <QJsonArray>
#include <QJsonObject>
#include "httpclient.h"
#include "tcpclient.h"
#include "usermanager.h"

SearchUserDialog::SearchUserDialog(QWidget *parent)
    : QDialog(parent)
    , m_searchInput(new QLineEdit(this))
    , m_searchByNameBtn(new QPushButton(tr("Search by Name"), this))
    , m_searchByIdBtn(new QPushButton(tr("Search by ID"), this))
    , m_resultList(new QListWidget(this))
    , m_addFriendBtn(new QPushButton(tr("Add Friend"), this))
    , m_closeBtn(new QPushButton(tr("Close"), this))
{
    setupUi();
    setWindowTitle(tr("Search User"));
    setFixedSize(400, 500);
}

SearchUserDialog::~SearchUserDialog() {}

void SearchUserDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(m_searchInput);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_searchByNameBtn);
    btnLayout->addWidget(m_searchByIdBtn);
    mainLayout->addLayout(btnLayout);

    mainLayout->addWidget(m_resultList);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_addFriendBtn);
    bottomLayout->addWidget(m_closeBtn);
    mainLayout->addLayout(bottomLayout);

    m_resultList->setSpacing(5);

    connect(m_searchByNameBtn, &QPushButton::clicked, this, &SearchUserDialog::onSearchByName);
    connect(m_searchByIdBtn, &QPushButton::clicked, this, &SearchUserDialog::onSearchById);
    connect(m_addFriendBtn, &QPushButton::clicked, this, &SearchUserDialog::onAddFriendClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void SearchUserDialog::onSearchByName()
{
    QString keyword = m_searchInput->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please enter a keyword!"));
        return;
    }

    QString token = UserManager::instance()->getCurrentToken();
    HttpClient::instance()->searchUser(keyword, 0, token,
        [this](const QJsonArray &users) {
        onSearchResultsReady(users);
    });
}

void SearchUserDialog::onSearchById()
{
    bool ok;
    int uid = m_searchInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Error"), tr("Please enter a valid user ID!"));
        return;
    }

    QString token = UserManager::instance()->getCurrentToken();
    QString keyword = QString::number(uid);
    HttpClient::instance()->searchUser(keyword, 1, token,
        [this](const QJsonArray &users) {
        onSearchResultsReady(users);
    });
}

void SearchUserDialog::onAddFriendClicked()
{
    QListWidgetItem *currentItem = m_resultList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a user!"));
        return;
    }

    int index = m_resultList->currentRow();
    if (index < 0 || index >= m_searchResults.size()) {
        return;
    }

    QJsonObject userJson = m_searchResults[index].toObject();
    int toUid = userJson["uid"].toInt();

    bool ok;
    QString message = QInputDialog::getText(this, tr("Add Friend"),
                                           tr("Enter verification message:"),
                                           QLineEdit::Normal, "", &ok);
    if (!ok) {
        return;
    }

    int fromUid = UserManager::instance()->getCurrentUid();
    TcpClient::instance()->sendAddFriendRequest(fromUid, toUid, message);

    QMessageBox::information(this, tr("Success"),
                           tr("Friend request sent!"));
}

void SearchUserDialog::onSearchResultsReady(const QJsonArray &users)
{
    m_resultList->clear();
    m_searchResults = users;

    for (const auto &userValue : users) {
        QJsonObject user = userValue.toObject();
        QString displayText = QString("%1 (ID: %2)")
            .arg(user["name"].toString())
            .arg(user["uid"].toInt());
        m_resultList->addItem(displayText);
    }

    if (users.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No users found!"));
    }
}
