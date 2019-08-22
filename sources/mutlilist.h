#ifndef MULTILIST_H
#define MULTILIST_H

#include <QComboBox>
#include <QAbstractItemView>

class QStandardItem;

class MultiListWidget
    : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(QStringList checkedItems READ checkedItems WRITE setCheckedItems)

public:
    MultiListWidget(QWidget *parent = nullptr);
    virtual ~MultiListWidget();

    QStringList checkedItems() const;
    void setCheckedItems(const QStringList &items);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    QStringList mCheckedItems;

    void collectCheckedItems();

    QString mDisplayText;
    const QRect mDisplayRectDelta;

    void updateDisplayText();

private slots:
    void slotModelRowsInserted(const QModelIndex &parent, int start, int end);
    void slotModelRowsRemoved(const QModelIndex &parent, int start, int end);
    void slotModelItemChanged(QStandardItem *item);

};

#endif // MULTILIST_H
