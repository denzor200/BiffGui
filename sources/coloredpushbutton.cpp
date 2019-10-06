#include "coloredpushbutton.h"

ColoredPushButton::ColoredPushButton(QWidget *parent) :
    QPushButton(parent)
{
}

void ColoredPushButton::mousePressEvent(QMouseEvent *event)
{

    // do something..

    QPushButton::mousePressEvent(event);
}


