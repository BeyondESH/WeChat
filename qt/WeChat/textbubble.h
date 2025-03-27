#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H
#include <QTextEdit>
#include <QHBoxLayout>
#include "bubbleframe.h"
class TextBubble:public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(MessageType type,QString &text,QWidget *parent=nullptr);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void adjustTextHeight();
    void setPlainText(const QString &text);
    void initStyleSheet();
    QTextEdit *_textEdit;
};

#endif // TEXTBUBBLE_H
