#include "maintabledelegates.h"
#include <QComboBox>
#include <QPainter>
#include <QList>
#include "libqxt\gui\qxtcheckcombobox.h"

#define RU_YES_TEXT "Да"
#define RU_NO_TEXT "Нет"
#define SEPARATOR ", "

CChoiceLinksDelegate::CChoiceLinksDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *CChoiceLinksDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QxtCheckComboBox* ccBox = new QxtCheckComboBox(parent);
    ccBox->setSeparator(SEPARATOR);
    return ccBox;
}

void CChoiceLinksDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QVariant& Data = index.model()->data(index);
    if (Data.canConvert<QList<QVariant>>())
    {
        QxtCheckComboBox *ccBox = qobject_cast<QxtCheckComboBox*> (editor);
        QList<QVariant> pair = Data.toList();
        QList<QVariant> CheckedList = pair[1].toList();

        ccBox->clear();
        ccBox->addItems(pair[0].toStringList());
        for (const QVariant& variantIndex : CheckedList)
        {
            ccBox->setItemCheckState(variantIndex.toInt(), Qt::Checked);
        }
    }
}

void CChoiceLinksDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QxtCheckComboBox *ccBox = qobject_cast<QxtCheckComboBox*> (editor);
    model->setData(index, ccBox->checkedItems());
}

void CChoiceLinksDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QVariant& Data = index.model()->data(index);
    if (Data.canConvert<QList<QVariant>>())
    {
        QList<QVariant> pair = Data.toList();
        QRect rect = option.rect;
        rect.setX(rect.x() + 5);
        rect.setWidth(rect.width() - 5);

        QString Value;
        QStringList names = pair[0].toStringList();
        QList<QVariant> ids = pair[1].toList();
        int cur_id = 0;
        for (const QString& name : names)
        {
            for (const QVariant& id : ids)
            {
                if (id.toInt() == cur_id)
                {
                    if (Value.size()!=0)
                        Value += SEPARATOR;
                    Value += name;
                }
            }
            cur_id++;
        }
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, Value);
    }
}

CChoiceDenyStatusDelegate::CChoiceDenyStatusDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *CChoiceDenyStatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (index.model()->data(index).canConvert<bool>())
    {
        QComboBox* comboBox = new QComboBox(parent);
        comboBox->addItems({RU_YES_TEXT, RU_NO_TEXT});
        return comboBox;
    }
    return nullptr;
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
    bool Value = false;
    const QVariant& Data = index.model()->data(index);
    if (Data.canConvert<bool>())
        Value = Data.toBool();
    QRect rect = option.rect;
    rect.setX(rect.x() + 5);
    rect.setWidth(rect.width() - 5);
    if (Value)
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, RU_YES_TEXT);
    else {
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, RU_NO_TEXT);
    }
}
