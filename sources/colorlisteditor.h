#ifndef COLORLISTEDITOR_H
#define COLORLISTEDITOR_H

#include <QComboBox>

class QColor;
class QWidget;

class ColorListEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor USER true)

public:
    ColorListEditor(QWidget *widget = 0);

signals:
    void colorChanged(const QColor& color);

public:
    QColor color() const;
    void setColor(QColor c);

private:
    void populateList();
    void _setColor(QColor c);
};

#endif
