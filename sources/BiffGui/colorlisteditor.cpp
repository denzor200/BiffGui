#include <QtGui>

#include "colorlisteditor.h"

ColorListEditor::ColorListEditor(QWidget *widget) : QComboBox(widget)
{
    populateList();
    setColor(Qt::white);
}

QColor ColorListEditor::color() const
{
    return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorListEditor::setColor(QColor c)
{
    //qDebug() << "ColorListEditor::setColor: " << c;
    setCurrentIndex(findData(c, int(Qt::DecorationRole)));
}

// #define NEED_COLOR_NAMES_TABLE

void ColorListEditor::populateList()
{
    // Поскольку библиотека Xceed.Words.NET требует строго только эти цвета - ничего не поделаешь
    static const QStringList colorNames = {
        "yellow",
        "green",
        "cyan",
        "magenta",
        "blue",
        "red",
        "darkBlue",
        "darkCyan",
        "darkGreen",
        "darkMagenta",
        "darkRed",
        // "darkYellow",
        "darkGray",
        "lightGray",
        "black",
        "white"
    };

#ifdef NEED_COLOR_NAMES_TABLE
        qDebug() << "-------------Begin color names table-------------";
#endif // NEED_COLOR_NAMES_TABLE

    for (int i = 0; i < colorNames.size(); ++i) {
        QColor color(colorNames[i]);

#ifdef NEED_COLOR_NAMES_TABLE
        qDebug()
                << "[" << colorNames[i] << ":" << color.name() << "]"
                << " = {" << color.red() << "," << color.green() << "," << color.blue() << "}";
#endif // NEED_COLOR_NAMES_TABLE

        insertItem(i, colorNames[i]);
        setItemData(i, color, Qt::DecorationRole);
    }

#ifdef NEED_COLOR_NAMES_TABLE
        qDebug() << "-------------End color names table-------------";
#endif // NEED_COLOR_NAMES_TABLE

}
