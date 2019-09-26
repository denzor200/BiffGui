#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
    class Settings;
}

class QDomDocument;

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
    void Initialize();
    int InitializeAssParsing( const QDomDocument & domDoc);
    int InitializeSrtParsing(const QDomDocument & domDoc);
    int InitializeTimingParsing(const QDomDocument & domDoc);
    int InitializeStyleParsing(const QDomDocument & domDoc);
    int InitializeStyle1Parsing(const QDomDocument & domDoc);
    int InitializeStyle2Parsing(const QDomDocument & domDoc);
    int InitializeStyle3Parsing(const QDomDocument & domDoc);
    int InitializeAdditionalParsing( const QDomDocument & domDoc);
private:
    Ui::Settings* ui;
};

#endif // SETTINGS_H
