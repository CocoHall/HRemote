#include <QStyledItemDelegate>
class NoFocusDelegate :public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
    {
        QStyleOptionViewItem itemOption(option);
        if (itemOption.state & QStyle::State_HasFocus){
            itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
        }
        QStyledItemDelegate::paint(painter, itemOption, index);
    }

};//用于将QTablewidget选中时的虚线框去掉

