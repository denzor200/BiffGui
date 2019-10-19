#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <functional>
#include <unordered_map>

namespace Ui {
    class Settings;
}

class QDomDocument;
class QXmlStreamWriter;
class QDomNode;

struct SettingsParsingStats
{
    QStringList Unrecognized;
    unsigned Count = 0;
};

class SettingsTable
{
public:
    using GetTextFuncType = std::function<QString(QWidget*)>;
    using ReaderFuncType = std::function<bool(QWidget*,const QList<QDomNode>&)>;
    using WriterFuncType = std::function<void(QWidget*,QStringList&)>;

private:
    struct Option
    {
        QString Path; // primary key

        QString CategoryName;
        QWidget* Label;
        QWidget* Widget;

        GetTextFuncType     GetTextFunc;
        ReaderFuncType      ReaderFunc;
        WriterFuncType      WriterFunc;

    };
    std::map<QString, Option> m_MainTable;
public:
    SettingsTable() = default;

    void Register(
            const QString& Path,
            const QString& Category,
            QWidget* Label,
            QWidget* Widget,
            ReaderFuncType ReaderFunc,
            WriterFuncType WriterFunc,
            GetTextFuncType GetTextFunc = nullptr);

    // Только для отладки
    void PrintFullTable() const;

    void Read(SettingsParsingStats* Stats, const QDomDocument &domDoc);
    void Write(QDomDocument& writer) const;

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
    bool commitChanges();

private slots:
    void on_toolButton_stackedWidgetPrev_clicked();
    void on_toolButton_stackedWidgetNext_clicked();

    void on_pushButton_Cancel_clicked();
    void on_pushButton_SetDefault_clicked();
    void on_pushButton_OK_clicked();
    void on_pushButton_Apply_clicked();

    void on_AnyValueEdited();

    void on_comboBox_PackingMethod_currentIndexChanged(int index);

private:
    bool _Initialize(bool FirstAttempt);
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
