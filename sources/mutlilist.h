#ifndef MULTILIST_H
#define MULTILIST_H

#include <QComboBox>
#include <QAbstractItemView>

class QStandardItem;

class HoverableComboBox : public QComboBox
{
    Q_OBJECT

public:
    HoverableComboBox(QWidget *parent) : QComboBox(parent)
    {
    }

    void showPopup() override
    {
        emit preopened();
        QComboBox::showPopup();
        emit opened();
    }
    void hidePopup() override
    {
        emit preclosed();
        QComboBox::hidePopup();
        emit closed();
    }

signals:
    void preopened();
    void preclosed();
    void opened();
    void closed();

};

class MultiListWidget
    : public HoverableComboBox
{
    Q_OBJECT

    Q_PROPERTY(QStringList checkedItems READ checkedItems WRITE setCheckedItems)

public:
    MultiListWidget(QWidget *parent = nullptr);
    virtual ~MultiListWidget();

    QStringList checkedItems() const;

    // Задавать статус "Checked" программно можно только через эти вызовы
    // Иначе рассинхронов с mCheckedItems не избежать!
    void setCheckedItems(const QStringList &items);
    void setCheckedItems(const std::vector<int>& ids);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QStringList mCheckedItems;

    void collectCheckedItems();

    QString mDisplayText;
    const QRect mDisplayRectDelta;

private slots:
    void slotModelRowsInserted(const QModelIndex &parent, int start, int end);
    void slotModelRowsRemoved(const QModelIndex &parent, int start, int end);
    void slotModelItemChanged(QStandardItem *item);

};

#endif // MULTILIST_H
