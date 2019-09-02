#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class MultiListPassiveWidget;
class QStandardItem;
class ActorName;

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget *parent = nullptr);

    MultiListPassiveWidget* GetList() {return m_List;}

    void CheckActor(const ActorName& name);
    void UncheckActor(const ActorName& name);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void slotListOpened();
    void slotEditingFinished();
    void slotCurrentIndexChanged(QStandardItem*);

private:
    MultiListPassiveWidget *m_List;
};

#endif // LIENEDIT_H
