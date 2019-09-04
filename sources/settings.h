#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
    class Dialog;
}

class Settings : public QDialog
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
private:
    Ui::Dialog* ui;
};

#endif // SETTINGS_H
