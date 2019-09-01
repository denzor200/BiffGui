/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class MultiListWidget;

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget *parent = nullptr);

    MultiListWidget* GetList() {return m_List;}

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void slotListOpened();

private:
    MultiListWidget *m_List;
};

#endif // LIENEDIT_H
