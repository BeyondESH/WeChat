#include "chatwindow.h"
#include <QFileDialog>
#include <QBuffer>
#include <QImageReader>
#include <QScrollBar>
#include <QDebug>
#include "usermanager.h"

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(new QScrollArea(this))
    , m_messageContainer(new QWidget(this))
    , m_messageLayout(new QVBoxLayout(m_messageContainer))
    , m_inputEdit(new QTextEdit(this))
    , m_sendBtn(new QPushButton(tr("Send"), this))
    , m_imageBtn(new QPushButton(tr("Image"), this))
    , m_titleLabel(new QLabel(this))
    , m_currentUid(0)
{
    m_currentUid = UserManager::instance()->getCurrentUid();
    setupUi();
}

ChatWindow::~ChatWindow() {}

void ChatWindow::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(m_titleLabel);

    m_messageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_messageLayout->setAlignment(Qt::AlignTop);
    m_messageLayout->setSpacing(10);

    m_scrollArea->setWidget(m_messageContainer);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(m_scrollArea);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_imageBtn);
    inputLayout->addWidget(m_inputEdit);
    inputLayout->addWidget(m_sendBtn);

    m_inputEdit->setMaximumHeight(100);
    m_inputEdit->setPlaceholderText(tr("Enter message..."));

    mainLayout->addLayout(inputLayout);

    connect(m_sendBtn, &QPushButton::clicked, this, &ChatWindow::onSendText);
    connect(m_imageBtn, &QPushButton::clicked, this, &ChatWindow::onSendImage);
    connect(m_inputEdit, &QTextEdit::returnPressed, this, &ChatWindow::onReturnPressed);
}

void ChatWindow::loadMessages(const QVector<MessageInfo> &messages)
{
    for (const MessageInfo &msg : messages) {
        bool isSelf = (msg.fromUid == m_currentUid);
        if (msg.msgType == 0) {
            appendTextMessage(msg, isSelf);
        } else if (msg.msgType == 1) {
            appendImageMessage(msg, isSelf);
        }
    }

    m_scrollArea->verticalScrollBar()->setValue(
        m_scrollArea->verticalScrollBar()->maximum());
}

void ChatWindow::appendMessage(const MessageInfo &message)
{
    bool isSelf = (message.fromUid == m_currentUid);
    if (message.msgType == 0) {
        appendTextMessage(message, isSelf);
    } else if (message.msgType == 1) {
        appendImageMessage(message, isSelf);
    }

    m_scrollArea->verticalScrollBar()->setValue(
        m_scrollArea->verticalScrollBar()->maximum());
}

void ChatWindow::onSendText()
{
    QString text = m_inputEdit->toPlainText().trimmed();
    if (text.isEmpty()) {
        return;
    }

    MessageInfo msg;
    msg.fromUid = m_currentUid;
    msg.content = text;
    msg.msgType = 0;

    appendTextMessage(msg, true);
    emit sendTextMessage(text);

    m_inputEdit->clear();
    m_scrollArea->verticalScrollBar()->setValue(
        m_scrollArea->verticalScrollBar()->maximum());
}

void ChatWindow::onSendImage()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Select Image"), "", tr("Images (*.png *.jpg *.jpeg *.gif *.bmp)"));

    if (filePath.isEmpty()) {
        return;
    }

    QImage image(filePath);
    if (image.isNull()) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to load image!"));
        return;
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    QString base64Data = QString::fromLatin1(byteArray.toBase64().data());

    QString imageType = QFileInfo(filePath).suffix().toLower();
    if (imageType == "jpg") imageType = "jpeg";

    MessageInfo msg;
    msg.fromUid = m_currentUid;
    msg.content = base64Data;
    msg.msgType = 1;
    msg.imageType = imageType;
    msg.imageWidth = image.width();
    msg.imageHeight = image.height();
    msg.fileSize = byteArray.size();

    appendImageMessage(msg, true);
    emit sendImageMessage(base64Data, imageType, image.width(), image.height(), byteArray.size());

    m_scrollArea->verticalScrollBar()->setValue(
        m_scrollArea->verticalScrollBar()->maximum());
}

void ChatWindow::onReturnPressed()
{
    if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
        m_inputEdit->append("");
    } else {
        onSendText();
    }
}

void ChatWindow::addBubble(QWidget *bubble)
{
    m_messageLayout->addWidget(bubble);
}

void ChatWindow::appendTextMessage(const MessageInfo &msg, bool isSelf)
{
    QString text = msg.content;
    QString time = msg.sendTime;

    QString displayText = QString("%1\n%2").arg(text).arg(time);
    MessageType type = isSelf ? MessageType::Self : MessageType::Other;

    TextBubble *bubble = new TextBubble(type, displayText, m_messageContainer);
    addBubble(bubble);
}

void ChatWindow::appendImageMessage(const MessageInfo &msg, bool isSelf)
{
    QString base64Data = msg.content;
    QByteArray imageData = QByteArray::fromBase64(base64Data.toLatin1());
    QPixmap pixmap;
    pixmap.loadFromData(imageData);

    if (pixmap.isNull()) {
        qDebug() << "Failed to load image from base64";
        return;
    }

    MessageType type = isSelf ? MessageType::Self : MessageType::Other;
    PictureBubble *bubble = new PictureBubble(pixmap, type, m_messageContainer);
    addBubble(bubble);
}
