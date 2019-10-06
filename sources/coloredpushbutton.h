#ifndef COLOREDPUSHBUTTON_H
#define COLOREDPUSHBUTTON_H

#include <QPushButton>

class ColoredPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColoredPushButton(QWidget *parent = nullptr);

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
};

#endif // COLOREDPUSHBUTTON_H
