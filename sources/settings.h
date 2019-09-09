#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
private slots:
    void on_toolButton_stackedWidgetPrev_clicked();
    void on_toolButton_stackedWidgetNext_clicked();

    void on_checkBox_DisableIntervals_stateChanged(int arg1);

private:
    Ui::Settings* ui;
};

#endif // SETTINGS_H
