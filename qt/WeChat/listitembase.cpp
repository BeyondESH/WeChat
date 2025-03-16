#include "listitembase.h"

ListItemBase::ListItemBase(QWidget *parent):QWidget(parent)
{

}

void ListItemBase::setItemTye(ListItemType type)
{
    _itemType=type;
}

ListItemType ListItemBase::getItemType()
{
    return _itemType;
}
