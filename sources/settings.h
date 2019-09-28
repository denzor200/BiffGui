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

    bool Initialize();
    bool Reset();

private slots:
    void on_toolButton_stackedWidgetPrev_clicked();
    void on_toolButton_stackedWidgetNext_clicked();

    void on_checkBox_DisableIntervals_stateChanged(int arg1);

    void on_pushButton_Cancel_clicked();

    void on_pushButton_SetDefault_clicked();

private:
    bool _Initialize(bool FirstAttempt);
    void InitializeAssParsing(void* stats, const QDomDocument & domDoc);
    void InitializeSrtParsing(void* stats,const QDomDocument & domDoc);
    void InitializeTimingParsing(void* stats,const QDomDocument & domDoc);
    void InitializeStyleParsing(void* stats,const QDomDocument & domDoc);
    void InitializeStyle1Parsing(void* stats,const QDomDocument & domDoc);
    void InitializeStyle2Parsing(void* stats,const QDomDocument & domDoc);
    void InitializeStyle3Parsing(void* stats,const QDomDocument & domDoc);
    void InitializeAdditionalParsing( void* stats,const QDomDocument & domDoc);
private:
    Ui::Settings* ui;
};

#endif // SETTINGS_H
