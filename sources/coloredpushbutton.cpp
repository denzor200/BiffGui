#include "coloredpushbutton.h"
#include <QColorDialog>
#include <QDebug>

ColoredPushButton::ColoredPushButton(QWidget *parent) :
    QPushButton(parent)
{
    SetColor(QColor(Qt::black));
}

void ColoredPushButton::SetColor(const QColor &color)
{
    // qDebug() << "ColoredPushButton::SetColor";
    setAutoFillBackground(true);
    setStyleSheet("background-color: " + color.name() + "");
    m_Color = color;
    m_ColorInitialized = true;
}

QColor ColoredPushButton::GetColor() const
{
    // qDebug() << "ColoredPushButton::GetColor";
    Q_ASSERT(m_ColorInitialized);
    return m_Color;
}

void ColoredPushButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);

    auto dialog = new QColorDialog(GetColor(), this);
    dialog->setWindowFlags( Qt::Widget );
    auto color = dialog->getColor();
    if (color.isValid())
    {
        SetColor(color);
    }
}


