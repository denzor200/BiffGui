#include "maintabledelegates.h"
#include <QPainter>
#include <QList>

#include <QComboBox>
#include "libqxt\gui\qxtcheckcombobox.h"
#include <QLineEdit>
#include <QStandardItemModel>

#include "config.h"
#include "settingsregistry.h"

#define RU_YES_TEXT "Да"
#define RU_NO_TEXT "Нет"
#define SEPARATOR ", "

CEnterNameDelegate::CEnterNameDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *CEnterNameDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return new QLineEdit(parent);
}

void CEnterNameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QVariant& Data = index.model()->data(index);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*> (editor);
    if (Data.canConvert<QString>())
        lineEdit->setText(Data.toString());
    else {
        lineEdit->setText("");
    }
}

void CEnterNameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*> (editor);
    model->setData(index, lineEdit->text());
}

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
    QxtCheckComboBox *ccBox = qobject_cast<QxtCheckComboBox*> (editor);
    ccBox->clear();
    if (Data.canConvert<QList<QVariant>>())
    {
        QList<QVariant> pair = Data.toList();
        QStringList List = pair[0].toStringList();
        QList<QVariant> CheckedList = pair[1].toList();
        QList<QVariant> CheckedOtherList = pair[2].toList();
        int offset = 0;

        if (!List.empty())
        {
            // Первый итем
            // Всегда комом..
            // Такой вот костыль..
            //////////////////////////////////////////////////////
            ccBox->addItem("");
            QStandardItemModel *model =
                  qobject_cast<QStandardItemModel *>(ccBox->model());
            Q_ASSERT(model != nullptr);
            QStandardItem *item = model->item(0);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            offset = 1;
            //////////////////////////////////////////////////////

            ccBox->addItems(List);
            for (const QVariant& variantIndex : CheckedList)
            {
                ccBox->setItemCheckState(variantIndex.toInt()+offset, Qt::Checked);
            }
            for (const QVariant& variantIndex : CheckedOtherList)
            {
                QFont font(ConfigHelpers::getString(SettingsRegistry::BUSY_ROLES_FONT));
                if (ConfigHelpers::getBoolean(SettingsRegistry::BUSY_ROLES_ITALIC))
                    font.setItalic(true);
                if (ConfigHelpers::getBoolean(SettingsRegistry::BUSY_ROLES_BOLD))
                    font.setBold(true);
                if (ConfigHelpers::getBoolean(SettingsRegistry::BUSY_ROLES_UNDERLINE))
                    font.setUnderline(true);
                font.setPointSize(ConfigHelpers::getInt(SettingsRegistry::BUSY_ROLES_SIZE));
                ccBox->setItemData(variantIndex.toInt()+offset, font, Qt::FontRole);
                ccBox->setItemData(variantIndex.toInt()+offset, QBrush(ConfigHelpers::getColor(SettingsRegistry::BUSY_ROLES_COLOR)), Qt::TextColorRole);
                ccBox->setItemData(variantIndex.toInt()+offset, QBrush(ConfigHelpers::getColor(SettingsRegistry::BUSY_ROLES_HIGHLIGHT)), Qt::BackgroundColorRole);
            }
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
                    break;
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
    // если editor еще не был создан, то эта функция никогда не вызовeтся
    // дополнительные проверки не нужны
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
    // если editor еще не был создан, то эта функция никогда не вызовeтся
    // дополнительные проверки не нужны
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
