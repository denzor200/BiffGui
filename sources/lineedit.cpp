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

#include "mutlilist.h"

#define EXPAND_BUTTON_WIDTH 25
#define EXPANDED_VIEW_MIN_WIDTH 125

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    clearButton = new MultiListWidget(this);
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setMaxVisibleItems(10);

    clearButton->setMaximumWidth(EXPAND_BUTTON_WIDTH);
    //clearButton->view()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    //determinge the maximum width required to display all names in full
    clearButton->view()->setMinimumWidth(EXPANDED_VIEW_MIN_WIDTH);



    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void LineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - EXPAND_BUTTON_WIDTH,
                      (rect().bottom() + 1 - sz.height())/2);
}



