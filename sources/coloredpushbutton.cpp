#include "coloredpushbutton.h"

ColoredPushButton::ColoredPushButton(QWidget *parent) :
    QPushButton(parent)
{
}

void ColoredPushButton::SetColor(const QColor &color)
{
    // TODO: implement this
}

QColor ColoredPushButton::GetColor() const
{
    // TODO: implement this
    return QColor("#FFFFFF");
}

void ColoredPushButton::mousePressEvent(QMouseEvent *event)
{

    // do something..

    QPushButton::mousePressEvent(event);
}


