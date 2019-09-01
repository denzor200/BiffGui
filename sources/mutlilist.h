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

class MultiListPassiveWidget
    : public HoverableComboBox
{
    Q_OBJECT

public:
    MultiListPassiveWidget(QWidget *parent = nullptr);
    virtual ~MultiListPassiveWidget();

    void setCheckedItems(const QStringList &items);
    void setCheckedItems(const std::vector<int>& ids);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QString mDisplayText;
    const QRect mDisplayRectDelta;

private slots:
    void slotModelRowsInserted(const QModelIndex &parent, int start, int end);

};

#endif // MULTILIST_H
