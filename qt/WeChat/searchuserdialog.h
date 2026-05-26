#ifndef SEARCHUSERDIALOG_H
#define SEARCHUSERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QInputDialog>

class SearchUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchUserDialog(QWidget *parent = nullptr);
    ~SearchUserDialog();

private slots:
    void onSearchByName();
    void onSearchById();
    void onAddFriendClicked();
    void onSearchResultsReady(const QJsonArray &users);

private:
    void setupUi();

    QLineEdit *m_searchInput;
    QPushButton *m_searchByNameBtn;
    QPushButton *m_searchByIdBtn;
    QListWidget *m_resultList;
    QPushButton *m_addFriendBtn;
    QPushButton *m_closeBtn;
    QJsonArray m_searchResults;
};

#endif // SEARCHUSERDIALOG_H
