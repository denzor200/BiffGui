#include "mutlilist.h"

#include <QStandardItemModel>
#include <QStylePainter>
#include <QStandardItem>

MultiListPassiveWidget::MultiListPassiveWidget(QWidget *parent)
    : HoverableComboBox(parent), mDisplayRectDelta(4, 1, -25, 0)
{
    setStyleSheet("QComboBox { combobox-popup: 1px }");
    connect(model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(slotModelRowsInserted(QModelIndex,int,int)));

}

MultiListPassiveWidget::~MultiListPassiveWidget()
{
}

void MultiListPassiveWidget::setCheckedItems(const QStringList &items)
{
    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel*>(model());

    // disconnect(standartModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));

    for (int i = 0; i < items.count(); ++i)
    {
        int index = findText(items.at(i));

        if (index != -1)
        {
            standartModel->item(index)->setData(Qt::Checked, Qt::CheckStateRole);
        }
    }

    // connect(standartModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));
}

void MultiListPassiveWidget::setCheckedItems(const std::vector<int>& ids)
{
    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel*>(model());

    // disconnect(standartModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));
    for (int id : ids)
    {
        // TODO: как быть, если индекс не корректен..
        // Пока снаружи гарантируется, что он всегда корректен
        standartModel->item(id)->setData(Qt::Checked, Qt::CheckStateRole);
    }
    // connect(standartModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));
}

void MultiListPassiveWidget::paintEvent(QPaintEvent *event)
{
    (void)event;

    QStylePainter painter(this);

    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox option;

    initStyleOption(&option);

    painter.drawComplexControl(QStyle::CC_ComboBox, option);

    /*QRect textRect = rect().adjusted(mDisplayRectDelta.left(), mDisplayRectDelta.top(),
                                     mDisplayRectDelta.right(), mDisplayRectDelta.bottom());

    painter.drawText(textRect, Qt::AlignVCenter, mDisplayText);*/
}

void MultiListPassiveWidget::slotModelRowsInserted(const QModelIndex &parent, int start, int end)
{
    (void)parent;

    QStandardItemModel *standartModel = qobject_cast<QStandardItemModel*>(model());

    // disconnect(standartModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));

    for (int i = start; i <= end; ++i)
    {
        standartModel->item(i)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        standartModel->item(i)->setData(Qt::Unchecked, Qt::CheckStateRole);
    }

    // connect(standartModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));
}
