#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class MultiListPassiveWidget;

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget *parent = nullptr);

    MultiListPassiveWidget* GetList() {return m_List;}

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void slotListOpened();
    void slotEditingFinished();
    void slotCurrentIndexChanged(const QString&);

private:
    MultiListPassiveWidget *m_List;
};

#endif // LIENEDIT_H
