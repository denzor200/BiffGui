#ifndef MAINTABLEDELEGATES_H
#define MAINTABLEDELEGATES_H

#include <QItemDelegate>

class CEnterNameDelegate : public QItemDelegate
{
public:
    CEnterNameDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const override;
};

class CChoiceLinksDelegate : public QItemDelegate
{
public:
    CChoiceLinksDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const override;

    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
};

class CChoiceDenyStatusDelegate : public QItemDelegate
{
public:
    CChoiceDenyStatusDelegate (QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const override;

    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
};

#endif // MAINTABLEDELEGATES_H
