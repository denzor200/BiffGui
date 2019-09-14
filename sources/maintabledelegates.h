#ifndef MAINTABLEDELEGATES_H
#define MAINTABLEDELEGATES_H

#include <QItemDelegate>

class CChoiceDenyStatusDelegate : public QItemDelegate
{
public:
    CChoiceDenyStatusDelegate (QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const;

    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // MAINTABLEDELEGATES_H
