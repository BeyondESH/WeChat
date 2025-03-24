#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent =nullptr);
    void appendChatItem(QWidget* item);//尾插
    void prependChatItem(QWidget* item);//头插
    void insertChatItem(QWidget* before, QWidget* item);//插入
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onVScrollBarMoved(int min,int max);
private:
    void initStyleSheet();

    QVBoxLayout* _layout;
    QScrollArea* _scrollArea;
    bool isLoading;
};

#endif // CHATVIEW_H
