#ifndef COLOREDPUSHBUTTON_H
#define COLOREDPUSHBUTTON_H

#include <QPushButton>

class ColoredPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColoredPushButton(QWidget *parent = nullptr);

    void setColor(const QColor& color);
    QColor color() const;


signals:
    void colorChanged(const QColor& color);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;

private:
    void _setColor(const QColor &color);

private:
    QColor m_Color;
    bool m_ColorInitialized = false;
};

#endif // COLOREDPUSHBUTTON_H
