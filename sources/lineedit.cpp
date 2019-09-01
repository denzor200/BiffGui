/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "lineedit.h"
#include <QToolButton>
#include <QStyle>

#include <QMenu>
#include <QDebug>

#include "actorslist.h"
#include "mutlilist.h"

#define EXPAND_BUTTON_WIDTH 25
#define EXPANDED_VIEW_MIN_WIDTH 125

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    m_List = new MultiListWidget(this);
    m_List->setCursor(Qt::ArrowCursor);
    m_List->setMaxVisibleItems(10);

    m_List->setMaximumWidth(EXPAND_BUTTON_WIDTH);
    //m_List->view()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    //determinge the maximum width required to display all names in full
    m_List->view()->setMinimumWidth(EXPANDED_VIEW_MIN_WIDTH);

    connect(m_List, SIGNAL(preopened()), this, SLOT(slotListOpened()));


    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(m_List->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), m_List->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), m_List->sizeHint().height() + frameWidth * 2 + 2));
}

void LineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = m_List->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    m_List->move(rect().right() - frameWidth - EXPAND_BUTTON_WIDTH,
                      (rect().bottom() + 1 - sz.height())/2);
}

void LineEdit::slotListOpened()
{
    auto actors = ActorsList::Inctance().lock();
    assert(actors);
    m_List->clear();
    for (const auto& Value : actors->Native())
    {
        m_List->addItem(Value.Get());
    }
}



