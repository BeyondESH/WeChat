#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QQueue>
#include "textbubble.h"
#include "picturebubble.h"

struct MessageInfo
{
    int messageId;
    int fromUid;
    int toUid;
    int groupId;
    QString content;
    QString sendTime;
    int msgType;
    int status;
    QString imageType;
    int imageWidth;
    int imageHeight;
    int fileSize;
};

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();
    void loadMessages(const QVector<MessageInfo> &messages);
    void appendMessage(const MessageInfo &message);

signals:
    void sendTextMessage(const QString &content);
    void sendImageMessage(const QString &base64Data, const QString &imageType,
                         int width, int height, int fileSize);

private slots:
    void onSendText();
    void onSendImage();
    void onReturnPressed();

private:
    void setupUi();
    void addBubble(QWidget *bubble);
    void appendTextMessage(const MessageInfo &msg, bool isSelf);
    void appendImageMessage(const MessageInfo &msg, bool isSelf);

    QScrollArea *m_scrollArea;
    QWidget *m_messageContainer;
    QVBoxLayout *m_messageLayout;
    QTextEdit *m_inputEdit;
    QPushButton *m_sendBtn;
    QPushButton *m_imageBtn;
    QLabel *m_titleLabel;
    QQueue<MessageInfo> m_pendingMessages;
    int m_currentUid;
};

#endif // CHATWINDOW_H
