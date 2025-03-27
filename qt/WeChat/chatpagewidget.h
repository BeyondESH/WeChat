#ifndef CHATPAGEWIDGET_H
#define CHATPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class chatPageWidget;
}

class ChatPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPageWidget(QWidget *parent = nullptr);
    ~ChatPageWidget();
    QWidget* getTitleWidget() const;
private slots:
    void on_quitPB_clicked();

    void on_minPB_clicked();

    void on_maxPB_toggled(bool checked);

    void on_topPB_toggled(bool checked);
    void on_sendPB_clicked();
private:
    Ui::chatPageWidget *ui;

    bool eventFilter(QObject *obj,QEvent *event) override;
protected:
    // void addListItem();
};

#endif // CHATPAGEWIDGET_H
