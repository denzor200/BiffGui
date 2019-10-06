#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <functional>

namespace Ui {
    class Settings;
}

class QDomDocument;

class SettingsTable
{
public:
    using GetTextFuncType = std::function<QString(QWidget*)>;

    struct Option
    {
        QString Path; // primary key

        QString CategoryName;
        QWidget* Label;
        QWidget* Widget;

        GetTextFuncType     GetTextFunc;

        // TODO: reader and writer

    };
    std::map<QString, Option> m_MainTable;
public:
    SettingsTable() = default;

    // Register with label
    void Register(const QString& Path, const QString& Category, QWidget* Label, QWidget* Widget, GetTextFuncType GetTextFunc = nullptr);

    // Register without label
    void Register(const QString& Path, const QString& Category, QWidget* Widget, GetTextFuncType GetTextFunc);

    // Только для отладки
    void PrintFullTable() const;

    QString GetFullNameByPath(const QString& Path) const;
};

class Settings : public QDialog
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    bool Initialize();
    bool Reset();

    void beginChanges();
    void commitChanges();

private slots:
    void on_toolButton_stackedWidgetPrev_clicked();
    void on_toolButton_stackedWidgetNext_clicked();

    void on_checkBox_DisableIntervals_stateChanged(int arg1);

    void on_pushButton_Cancel_clicked();
    void on_pushButton_SetDefault_clicked();
    void on_pushButton_OK_clicked();
    void on_pushButton_Apply_clicked();

    void on_spinBox_RoundValue_valueChanged(int arg1);
    void on_spinBox_SmallInterval_valueChanged(int arg1);
    void on_spinBox_NormalInterval_valueChanged(int arg1);
    void on_spinBox_BigInterval_valueChanged(int arg1);
    void on_spinBox_VeryBigInterval_valueChanged(int arg1);
    void on_spinBox_VeryVeryBigInterval_valueChanged(int arg1);
    void on_lineEdit_Font_textChanged(const QString &arg1);
    void on_doubleSpinBox_Size_valueChanged(double arg1);
    void on_lineEdit_Font_2_textChanged(const QString &arg1);
    void on_doubleSpinBox_Size_2_valueChanged(double arg1);
    void on_lineEdit_Font_3_textChanged(const QString &arg1);
    void on_doubleSpinBox_Size_3_valueChanged(double arg1);
    void on_checkBox_DisableTags_stateChanged(int arg1);
    void on_checkBox_DisableCounter_stateChanged(int arg1);
    void on_checkBox_ExtSound_stateChanged(int arg1);
    void on_comboBox_Distribution_currentIndexChanged(int index);
    void on_lineEdit_ColumnText_textChanged(const QString &arg1);
    void on_lineEdit_ColumnStart_textChanged(const QString &arg1);
    void on_lineEdit_ColumnEnd_textChanged(const QString &arg1);
    void on_lineEdit_ColumnName_textChanged(const QString &arg1);

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

    void InitializeMainTable();

    void on_CommitedChanges();

private:
    struct ConfigOptionData
    {
        QString Path;

    };

    SettingsTable m_Table;
    Ui::Settings* ui;
};

#endif // SETTINGS_H
