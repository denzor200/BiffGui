#include "maintabledelegates.h"
#include <QComboBox>
#include <QPainter>
#include <QApplication>

#define RU_YES_TEXT "Да"
#define RU_NO_TEXT "Нет"

CChoiceDenyStatusDelegate::CChoiceDenyStatusDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *CChoiceDenyStatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox* comboBox = new QComboBox(parent);
    comboBox->addItems({RU_YES_TEXT, RU_NO_TEXT});
    return comboBox;
}

void CChoiceDenyStatusDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    bool value = index.model()->data(index).toBool();
    QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
    if (value)
        comboBox->setCurrentIndex(0);
    else {
        comboBox->setCurrentIndex(1);
    }
}

void CChoiceDenyStatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
    if (comboBox->currentIndex() == 0)
        model->setData(index, true);
    else if (comboBox->currentIndex() == 1)
    {
        model->setData(index, false);
    }
}

void CChoiceDenyStatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool value = index.model()->data(index).toBool();
    QRect rect = option.rect;
    rect.setX(rect.x() + 5);
    if (value)
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, RU_YES_TEXT);
    else {
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, RU_NO_TEXT);
    }
}
