#ifndef FRIENDREQUESTDIALOG_H
#define FRIENDREQUESTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class FriendRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FriendRequestDialog(int fromUid, const QString &fromName,
                               const QString &message, QWidget *parent = nullptr);
    ~FriendRequestDialog();
    bool isAccepted() const { return m_accepted; }

private slots:
    void onAccept();
    void onReject();

private:
    void setupUi();

    int m_fromUid;
    QString m_fromName;
    QString m_message;
    bool m_accepted;
};

#endif // FRIENDREQUESTDIALOG_H
