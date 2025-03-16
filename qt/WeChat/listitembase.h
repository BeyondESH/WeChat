#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QWidget>
#include "global.h"
class ListItemBase :public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget *parent=nullptr);
    void setItemTye(ListItemType type);
    ListItemType getItemType();
private:
    ListItemType _itemType;

public slots:

signals:
};

#endif // LISTITEMBASE_H
