#include "searchuserdialog.h"
#include "ui_searchuserdialog.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include <QMessageBox>
#include "chatuserwidget.h"

SearchUserDialog::SearchUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchUserDialog)
{
    ui->setupUi(this);
    
    // 设置无边框窗口
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    
    // 连接信号槽
    auto tcpMgr = TCPMgr::getInstance();
    connect(tcpMgr.get(), &TCPMgr::signal_search_user_success, this, &SearchUserDialog::onSearchSuccess);
    connect(tcpMgr.get(), &TCPMgr::signal_search_user_failed, this, &SearchUserDialog::onSearchFailed);
    connect(tcpMgr.get(), &TCPMgr::signal_add_friend_success, this, &SearchUserDialog::onAddFriendSuccess);
    connect(tcpMgr.get(), &TCPMgr::signal_add_friend_failed, this, &SearchUserDialog::onAddFriendFailed);
    
    // 连接列表项点击信号
    connect(ui->resultList, &QListWidget::itemClicked, this, &SearchUserDialog::onUserItemClicked);
    
    // 默认隐藏结果区域
    ui->resultArea->setVisible(false);
}

SearchUserDialog::~SearchUserDialog()
{
    delete ui;
}

void SearchUserDialog::showDialog()
{
    // 清空上次搜索结果
    ui->searchLE->clear();
    clearResults();
    ui->resultArea->setVisible(false);
    
    // 显示对话框
    show();
}

void SearchUserDialog::on_searchPB_clicked()
{
    QString keyword = ui->searchLE->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, tr("搜索用户"), tr("请输入用户名或ID"));
        return;
    }
    
    // 发送搜索请求
    QJsonObject searchObj;
    searchObj["keyword"] = keyword;
    searchObj["searchType"] = keyword.toInt() > 0 ? 1 : 0;  // 1表示按ID搜索，0表示按用户名搜索
    
    TCPMgr::getInstance()->slot_send_data(ReqId::ID_SEARCH_USER_REQ, QString(QJsonDocument(searchObj).toJson()));
    
    // 显示加载状态
    ui->resultArea->setVisible(true);
    ui->resultList->clear();
    ui->resultList->addItem(tr("正在搜索..."));
}

void SearchUserDialog::on_cancelPB_clicked()
{
    close();
}

void SearchUserDialog::onSearchSuccess(const QVector<UserInfo>& users)
{
    _searchResults = users;
    clearResults();
    
    // 显示搜索结果
    if (users.isEmpty()) {
        ui->resultList->addItem(tr("未找到符合条件的用户"));
        return;
    }
    
    for (const auto& user : users) {
        // 跳过自己
        if (user.uid == UserMgr::getInstance()->getUid()) {
            continue;
        }
        
        // 创建用户项
        ChatUserWidget* userWidget = new ChatUserWidget(ui->resultList);
        userWidget->setInfo(user.name, 
                         user.avatar.isEmpty() ? ":/img/loginDialog/img/avatar/default.jpg" : user.avatar, 
                         QDateTime(), "");
        userWidget->setUserData(user.uid);
        
        QListWidgetItem* item = new QListWidgetItem(ui->resultList);
        item->setSizeHint(userWidget->sizeHint());
        ui->resultList->addItem(item);
        ui->resultList->setItemWidget(item, userWidget);
    }
}

void SearchUserDialog::onSearchFailed(int error)
{
    clearResults();
    ui->resultList->addItem(tr("搜索失败，错误码：%1").arg(error));
}

void SearchUserDialog::onUserItemClicked(QListWidgetItem* item)
{
    ChatUserWidget* widget = qobject_cast<ChatUserWidget*>(ui->resultList->itemWidget(item));
    if (!widget) {
        return;
    }
    
    int uid = widget->getUserData();
    
    // 查找对应的用户信息
    UserInfo selectedUser;
    for (const auto& user : _searchResults) {
        if (user.uid == uid) {
            selectedUser = user;
            break;
        }
    }
    
    // 发送选中的用户信息
    emit friendSelected(selectedUser);
    
    // 关闭对话框
    accept();
}

void SearchUserDialog::onAddFriendSuccess()
{
    QMessageBox::information(this, tr("添加好友"), tr("好友请求已发送，等待对方确认"));
    close();
}

void SearchUserDialog::onAddFriendFailed(int error)
{
    QMessageBox::warning(this, tr("添加好友"), tr("发送好友请求失败，错误码：%1").arg(error));
}

void SearchUserDialog::clearResults()
{
    ui->resultList->clear();
}
