#include "settings.h"
#include "ui_settings.h"
#include <QtXml>
#include <QDebug>

// TODO: make normal path
#define SETTINGS_DIR "D:/repos/subtitles/Debug/settings.xml"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    Initialize();
}

Settings::~Settings()
{
    delete ui;
}


void Settings::on_toolButton_stackedWidgetPrev_clicked()
{
    int current = ui->stackedWidget->currentIndex();
    int count = ui->stackedWidget->count();
    if (current == -1 || count == 0)
        return;
    if (current == 0)
        ui->stackedWidget->setCurrentIndex(count-1);
    else {
        ui->stackedWidget->setCurrentIndex(current-1);
    }
}

void Settings::on_toolButton_stackedWidgetNext_clicked()
{
    int current = ui->stackedWidget->currentIndex();
    int count = ui->stackedWidget->count();
    if (current == -1 || count == 0)
        return;
    if (current == count-1)
        ui->stackedWidget->setCurrentIndex(0);
    else
        ui->stackedWidget->setCurrentIndex(current+1);
}



void Settings::on_checkBox_DisableIntervals_stateChanged(int arg1)
{
    switch (static_cast<Qt::CheckState>(arg1))
    {
    case Qt::CheckState::Checked:
        ui->spinBox_SmallInterval->setEnabled(false);
        ui->spinBox_NormalInterval->setEnabled(false);
        ui->spinBox_BigInterval->setEnabled(false);
        ui->spinBox_VeryBigInterval->setEnabled(false);
        ui->spinBox_VeryVeryBigInterval->setEnabled(false);
        ui->label_SmallInterval->setEnabled(false);
        ui->label_NormalInterval->setEnabled(false);
        ui->label_BigInterval->setEnabled(false);
        ui->label_VeryBigInterval->setEnabled(false);
        ui->label_VeryVeryBigInterval->setEnabled(false);
        break;
    case Qt::CheckState::Unchecked:
        ui->spinBox_SmallInterval->setEnabled(true);
        ui->spinBox_NormalInterval->setEnabled(true);
        ui->spinBox_BigInterval->setEnabled(true);
        ui->spinBox_VeryBigInterval->setEnabled(true);
        ui->spinBox_VeryVeryBigInterval->setEnabled(true);
        ui->label_SmallInterval->setEnabled(true);
        ui->label_NormalInterval->setEnabled(true);
        ui->label_BigInterval->setEnabled(true);
        ui->label_VeryBigInterval->setEnabled(true);
        ui->label_VeryVeryBigInterval->setEnabled(true);
        break;
    default:
        Q_ASSERT_X(0, "checkBoxStateChanged",  "Invalid 'Qt::CheckState'");
        break;
    }
}

void Settings::Initialize()
{
    QDomDocument domDoc;
    QFile file(SETTINGS_DIR);
    // TODO: бросить исключение, если файл не найден (не забыть перед этим сделать запрос на повторное создание файла)
    if (file.open(QIODevice::ReadOnly))
    {
        QString errorStr;
        int errorLine;
        int errorColumn;
        if (domDoc.setContent(&file, true, &errorStr, &errorLine,
                              &errorColumn))
        {
            int count = 0;
            count += InitializeAssParsing(domDoc);
            count += InitializeSrtParsing(domDoc);
            count += InitializeTimingParsing(domDoc);
            count += InitializeStyleParsing(domDoc);
            count += InitializeAdditionalParsing(domDoc);
        }
        else {
            qWarning("Line %d, column %d: %s", errorLine, errorColumn,
                         errorStr.toUtf8().data());
        }
    }
}


template <typename T>
static QList<QDomNode> elementsByTagPath(const T& doc, QStringList& names)
{
    QList<QDomNode> Returned;
    if (!names.empty())
    {
        QString name = names[0];
        QDomNodeList nodes = doc.elementsByTagName(name);
        if (names.size()==1)
        {
            for (int x = 0; x < nodes.count(); x++)
            {
                QDomElement node = nodes.at(x).toElement();
                Returned += node;
            }
        }
        else {
            for (int x = 0; x < nodes.count(); x++)
            {
                QDomElement node = nodes.at(x).toElement();
                names.pop_front();
                Returned += elementsByTagPath(node, names);
            }
        }
    }
    return Returned;
}

template <typename T>
static QList<QDomNode> elementsByTagPath(const T& doc, const QString& Path)
{
    QStringList names = Path.split(".");
    return elementsByTagPath(doc, names);
}

struct ParsingStats
{
    QStringList Unrecognized;
    int Count = 0;
};

static void AssParsingInitializeNeededColumns(ParsingStats* stats,const QDomDocument &domDoc, QLineEdit* lineEdit, const QString& Path)
{
    QString text;
    QList<QDomNode> nodes = elementsByTagPath(domDoc, Path);
    Q_ASSERT(stats);
    Q_ASSERT(lineEdit);
    if (!nodes.empty())
    {
        for (const QDomNode& node : nodes)
        {
            const QString& nodeText = node.toElement().text();
            if (text.size()!=0&&nodeText.size()!=0)
                text += ", ";
            text += nodeText;
        }
        if (text.size()!=0)
        {
            lineEdit->setText(text);
            stats->Count++;
        }
        else {
            stats->Unrecognized.push_back(Path);
        }
    }
    else {
        stats->Unrecognized.push_back(Path);
    }
}

static void SrtParsingInitializeTimeDistributing(ParsingStats* stats,const QDomDocument &domDoc, QComboBox* comboBox, const QString& Path)
{
    QList<QDomNode> nodes = elementsByTagPath(domDoc, Path);
    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText == "Uniform")
        {
            comboBox->setCurrentIndex(0);
            stats->Count++;
        }
        else if (nodeText == "Fair")
        {
            comboBox->setCurrentIndex(1);
            stats->Count++;
        }
        else {
            stats->Unrecognized.push_back(Path);
        }
    }
    else {
        stats->Unrecognized.push_back(Path);
    }
}

static void InitializeSpinBox(ParsingStats* stats,const QDomDocument &domDoc, QSpinBox* spinBox, const QString& Path)
{
    QList<QDomNode> nodes = elementsByTagPath(domDoc, Path);
    if (nodes.size() == 1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        int iValue = 0;
        bool castStatus = false;
        iValue = nodeText.toInt(&castStatus);
        if (castStatus)
        {
            spinBox->setValue(iValue);
            stats->Count++;
        }
        else {
            stats->Unrecognized.push_back(Path);
        }
    }
    else {
        stats->Unrecognized.push_back(Path);
    }
}

static void InitializeDoubleSpinBox(ParsingStats* stats,const QDomDocument &domDoc, QDoubleSpinBox* spinBox, const QString& Path)
{
    QList<QDomNode> nodes = elementsByTagPath(domDoc, Path);
    if (nodes.size() == 1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        double dValue = 0;
        bool castStatus = false;
        dValue = nodeText.toDouble(&castStatus);
        if (castStatus)
        {
            spinBox->setValue(dValue);
            stats->Count++;
        }
        else {
            stats->Unrecognized.push_back(Path);
        }
    }
    else {
        stats->Unrecognized.push_back(Path);
    }
}

static void InitializeCheckBox(ParsingStats* stats,const QDomDocument &domDoc, QCheckBox* checkBox, const QString& Path)
{
    QList<QDomNode> nodes = elementsByTagPath(domDoc, Path);
    if (nodes.size() == 1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText=="true")
        {
            checkBox->setCheckState(Qt::CheckState::Checked);
            stats->Count++;
        }
        else if (nodeText=="false")
        {
            checkBox->setCheckState(Qt::CheckState::Unchecked);
            stats->Count++;
        }
        else {
            stats->Unrecognized.push_back(Path);
        }
    }
    else {
        stats->Unrecognized.push_back(Path);
    }
}

static void InitializeLineEdit(ParsingStats* stats,const QDomDocument &domDoc, QLineEdit* lineEdit, const QString& Path)
{
    QList<QDomNode> nodes = elementsByTagPath(domDoc, Path);
    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText.size()!=0)
        {
            lineEdit->setText(nodeText);
            stats->Count++;
        }
        else {
            stats->Unrecognized.push_back(Path);
        }
    }
    else {
        stats->Unrecognized.push_back(Path);
    }
}


int Settings::InitializeAssParsing(const QDomDocument &domDoc)
{
    // TODO: запретить запятые
    ParsingStats stats;
    AssParsingInitializeNeededColumns(&stats, domDoc, ui->lineEdit_ColumnStart, "Root.AssParsing.NeededColumns.TimeStartColumn");
    AssParsingInitializeNeededColumns(&stats, domDoc, ui->lineEdit_ColumnEnd, "Root.AssParsing.NeededColumns.TimeEndColumn");
    AssParsingInitializeNeededColumns(&stats, domDoc, ui->lineEdit_ColumnName, "Root.AssParsing.NeededColumns.NameColumn");
    AssParsingInitializeNeededColumns(&stats, domDoc, ui->lineEdit_ColumnText, "Root.AssParsing.NeededColumns.TextColumn");
    return stats.Count;
}

int Settings::InitializeSrtParsing(const QDomDocument & domDoc)
{
    ParsingStats stats;
    SrtParsingInitializeTimeDistributing(&stats, domDoc, ui->comboBox_Distribution, "Root.SrtParsing.ComplexPhrases.TimeDistributing.Strategy");
    return stats.Count;
}

int Settings::InitializeTimingParsing(const QDomDocument &domDoc)
{
    ParsingStats stats;
    InitializeSpinBox(&stats, domDoc, ui->spinBox_RoundValue, "Root.Timing.MilisecondRoundValue");
    InitializeCheckBox(&stats, domDoc, ui->checkBox_DisableIntervals, "Root.Timing.DisableIntervals");
    InitializeSpinBox(&stats, domDoc, ui->spinBox_SmallInterval, "Root.Timing.IntervalsDistributing.VerySmall");
    InitializeSpinBox(&stats, domDoc, ui->spinBox_NormalInterval, "Root.Timing.IntervalsDistributing.Normal");
    InitializeSpinBox(&stats, domDoc, ui->spinBox_BigInterval, "Root.Timing.IntervalsDistributing.Big");
    InitializeSpinBox(&stats, domDoc, ui->spinBox_VeryBigInterval, "Root.Timing.IntervalsDistributing.VeryBig");
    InitializeSpinBox(&stats, domDoc, ui->spinBox_VeryVeryBigInterval, "Root.Timing.IntervalsDistributing.VeryVeryBig");
    return stats.Count;
}

int Settings::InitializeStyleParsing(const QDomDocument & domDoc)
{
    ParsingStats stats;
    InitializeCheckBox(&stats, domDoc, ui->checkBox_DisableTags, "Root.DocumentStyle.DisableTags");
    InitializeCheckBox(&stats, domDoc, ui->checkBox_DisableCounter, "Root.DocumentStyle.DisableLinesCounter");

    stats.Count += InitializeStyle1Parsing(domDoc);
    stats.Count += InitializeStyle2Parsing(domDoc);
    stats.Count += InitializeStyle3Parsing(domDoc);
    return stats.Count;
}

int Settings::InitializeStyle1Parsing(const QDomDocument &domDoc)
{
    ParsingStats stats;
    InitializeLineEdit(&stats, domDoc, ui->lineEdit_Font, "Root.DocumentStyle.MainFont.Name");
    InitializeDoubleSpinBox(&stats, domDoc, ui->doubleSpinBox_Size, "Root.DocumentStyle.MainFont.Size");
    return stats.Count;
}

int Settings::InitializeStyle2Parsing(const QDomDocument &domDoc)
{
    ParsingStats stats;
    InitializeLineEdit(&stats, domDoc, ui->lineEdit_Font_2, "Root.DocumentStyle.PageNumberFont.Name");
    InitializeDoubleSpinBox(&stats, domDoc, ui->doubleSpinBox_Size_2, "Root.DocumentStyle.PageNumberFont.Size");
    return stats.Count;
}

int Settings::InitializeStyle3Parsing(const QDomDocument &domDoc)
{
    ParsingStats stats;
    InitializeLineEdit(&stats, domDoc, ui->lineEdit_Font_3, "Root.DocumentStyle.IndividualSelectedFont.Name");
    InitializeDoubleSpinBox(&stats, domDoc, ui->doubleSpinBox_Size_3, "Root.DocumentStyle.IndividualSelectedFont.Size");
    return stats.Count;
}

int Settings::InitializeAdditionalParsing(const QDomDocument &domDoc)
{
    ParsingStats stats;
    InitializeCheckBox(&stats, domDoc, ui->checkBox_ExtSound, "Root.Additional.EnableSoundTheme");
    return stats.Count;
}
