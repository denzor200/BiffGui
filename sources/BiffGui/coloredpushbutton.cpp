#include "coloredpushbutton.h"
#include <QColorDialog>
#include <QDebug>

ColoredPushButton::ColoredPushButton(QWidget *parent) :
    QPushButton(parent)
{
    _setColor(QColor(Qt::black));
}

void ColoredPushButton::setColor(const QColor &color)
{
    // qDebug() << "ColoredPushButton::setColor";
    _setColor(color);
    emit colorChanged(m_Color);
}

QColor ColoredPushButton::color() const
{
    // qDebug() << "ColoredPushButton::color";
    Q_ASSERT(m_ColorInitialized);
    return m_Color;
}

void ColoredPushButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);

    auto dialog = new QColorDialog(color(), this);
    dialog->setWindowFlags( Qt::Widget );
    auto color = dialog->getColor();
    if (color.isValid())
    {
        setColor(color);
    }
}

void ColoredPushButton::_setColor(const QColor &color)
{
    setAutoFillBackground(true);
    setStyleSheet("background-color: " + color.name() + "");
    m_Color = color;
    m_ColorInitialized = true;
}


