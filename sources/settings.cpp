#include "settings.h"
#include "ui_settings.h"
#include <QtXml>
#include <QDebug>
#include <QMessageBox>
#include <sstream>
#include "converterwaiting.h"

// TODO: make normal path
#define SETTINGS_DIR "settings.xml"

static QString PrintConfigPath(const QString& Path)
{
    // AssParsing
    if (Path == "Root.AssParsing.NeededColumns.TimeStartColumn")
        return "ASS:Колонка Start";
    else if (Path == "Root.AssParsing.NeededColumns.TimeEndColumn")
        return "ASS:Колонка End";
    else if (Path == "Root.AssParsing.NeededColumns.NameColumn")
        return "ASS:Колонка Name";
    else if (Path == "Root.AssParsing.NeededColumns.TextColumn")
        return "ASS:Колонка Text";


    // SrtParsing
    else if (Path == "Root.SrtParsing.ComplexPhrases.TimeDistributing.Strategy")
        return "SRT:Метод распределения времени в комплексных фразах";


    // Timing
    else if (Path == "Root.Timing.MilisecondRoundValue")
        return "Тайминг:Верхний порог округления";
    else if (Path == "Root.Timing.DisableIntervals")
        return "Тайминг:Отключить интервалы";
    else if (Path == "Root.Timing.IntervalsDistributing.VerySmall")
        return "Тайминг:Маленький интервал";
    else if (Path == "Root.Timing.IntervalsDistributing.Normal")
        return "Тайминг:Обычный интервал";
    else if (Path == "Root.Timing.IntervalsDistributing.Big")
        return "Тайминг:Большой интервал";
    else if (Path == "Root.Timing.IntervalsDistributing.VeryBig")
        return "Тайминг:Очень большой интервал";
    else if (Path == "Root.Timing.IntervalsDistributing.VeryVeryBig")
        return "Тайминг:Слишком большой интервал";


    // DocumentStyle
    else if (Path == "Root.DocumentStyle.DisableTags")
        return "Стиль документа:Отключить тэги";
    else if (Path == "Root.DocumentStyle.DisableLinesCounter")
        return "Стиль документа:Отключить подсчет строк";

    else if (Path == "Root.DocumentStyle.MainFont.Name")
        return "Стиль документа:Главное:Шрифт";
    else if (Path == "Root.DocumentStyle.MainFont.Size")
        return "Стиль документа:Главное:Размер";

    else if (Path == "Root.DocumentStyle.PageNumberFont.Name")
        return "Стиль документа:Нумерация:Шрифт";
    else if (Path == "Root.DocumentStyle.PageNumberFont.Size")
        return "Стиль документа:Нумерация:Размер";

    else if (Path == "Root.DocumentStyle.IndividualSelectedFont.Name")
        return "Стиль документа:Выделенные:Шрифт";
    else if (Path =="Root.DocumentStyle.IndividualSelectedFont.Size")
        return "Стиль документа:Выделенные:Размер";

    // Additional
    else if (Path == "Root.Additional.EnableSoundTheme")
        return "Дополнительно:Включить расширенную звуковую тему";


    return Path;
}

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

bool Settings::Initialize()
{
    bool Status = _Initialize(true);
    ui->pushButton_OK->setEnabled(false);
    ui->pushButton_Apply->setEnabled(false);
    ui->pushButton_Cancel->setFocus();
    return Status;
}

bool Settings::Reset()
{
    ConverterWaiting_ResetSetting waiting;
    waiting.StartProcess();
    int execStatus = waiting.exec();
    bool isCanceled = waiting.IsCanceledByUser();
    int procStatus = waiting.GetProcessStatus();
    Q_ASSERT(!isCanceled);

    if (0 == execStatus && 0 == procStatus)
    {
    }
    else {
        QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось сбросить настройки по умолчанию");
    }
    return _Initialize(false);
}

void Settings::beginChanges()
{
    ui->pushButton_OK->setEnabled(true);
    ui->pushButton_Apply->setEnabled(true);
    //ui->pushButton_Apply->setFocus();
}

static QString CheckBoxGetText(QCheckBox* checkBox)
{
    switch (checkBox->checkState())
    {
    case Qt::CheckState::Checked:
        return "true";
    case Qt::CheckState::Unchecked:
        return "false";
    default:
        Q_ASSERT_X(0, "CheckBoxGetText",  "Invalid 'Qt::CheckState'");
        break;
    }
    return "";
}

static void WriteAssColumnAttribute(QXmlStreamWriter& xmlWriter, const QString& attributeName, QLineEdit* lineEdit)
{
    const QString& text = lineEdit->text();
    QStringList sections = text.split(",", QString::SkipEmptyParts);
    for (const QString& section : sections)
    {
        xmlWriter.writeTextElement(attributeName,section.trimmed());
    }
}

void Settings::commitChanges()
{
    QFile file(SETTINGS_DIR);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Root");
    {
        xmlWriter.writeStartElement("AssParsing");
        {
            xmlWriter.writeStartElement("NeededColumns");
            {
                WriteAssColumnAttribute(xmlWriter,"NameColumn",ui->lineEdit_ColumnName);
                WriteAssColumnAttribute(xmlWriter,"TextColumn",ui->lineEdit_ColumnText);
                WriteAssColumnAttribute(xmlWriter,"TimeStartColumn",ui->lineEdit_ColumnStart);
                WriteAssColumnAttribute(xmlWriter,"TimeEndColumn",ui->lineEdit_ColumnEnd);
            } xmlWriter.writeEndElement();
        } xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("Timing");
        {
            xmlWriter.writeTextElement("MilisecondRoundValue",ui->spinBox_RoundValue->text());
            xmlWriter.writeTextElement("DisableIntervals",CheckBoxGetText(ui->checkBox_DisableIntervals));
            xmlWriter.writeStartElement("IntervalsDistributing");
            {
                xmlWriter.writeTextElement("VerySmall",ui->spinBox_SmallInterval->text());
                xmlWriter.writeTextElement("Normal",ui->spinBox_NormalInterval->text());
                xmlWriter.writeTextElement("Big",ui->spinBox_BigInterval->text());
                xmlWriter.writeTextElement("VeryBig",ui->spinBox_VeryBigInterval->text());
                xmlWriter.writeTextElement("VeryVeryBig",ui->spinBox_VeryVeryBigInterval->text());
            } xmlWriter.writeEndElement();
        } xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("DocumentStyle");
        {
            xmlWriter.writeTextElement("DisableTags",CheckBoxGetText(ui->checkBox_DisableTags));
            xmlWriter.writeTextElement("DisableLinesCounter",CheckBoxGetText(ui->checkBox_DisableCounter));
            xmlWriter.writeStartElement("MainFont");
            {
                xmlWriter.writeTextElement("Name",ui->lineEdit_Font->text());
                xmlWriter.writeTextElement("Size",QString::number(ui->doubleSpinBox_Size->value()));
            } xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("PageNumberFont");
            {
                xmlWriter.writeTextElement("Name",ui->lineEdit_Font_2->text());
                xmlWriter.writeTextElement("Size",QString::number(ui->doubleSpinBox_Size_2->value()));
            } xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("IndividualSelectedFont");
            {
                xmlWriter.writeTextElement("Name",ui->lineEdit_Font_3->text());
                xmlWriter.writeTextElement("Size",QString::number(ui->doubleSpinBox_Size_3->value()));
            } xmlWriter.writeEndElement();
        } xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("SrtParsing");
        {
            xmlWriter.writeStartElement("ComplexPhrases");
            {
                xmlWriter.writeStartElement("TimeDistributing");
                {
                    xmlWriter.writeTextElement("Strategy",ui->comboBox_Distribution->currentText());
                } xmlWriter.writeEndElement();
            } xmlWriter.writeEndElement();
        } xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("Additional");
        {
            xmlWriter.writeTextElement("EnableSoundTheme",CheckBoxGetText(ui->checkBox_ExtSound));
        } xmlWriter.writeEndElement();

    } xmlWriter.writeEndElement();

    xmlWriter.writeEndDocument();
    file.close();

    this->on_CommitedChanges();
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

    beginChanges();
}

struct ParsingStats
{
    QStringList Unrecognized;
    unsigned Count = 0;
};

bool Settings::_Initialize(bool FirstAttempt)
{
    QDomDocument domDoc;
    QFile file(SETTINGS_DIR);

    auto HandleError = [&]() -> bool
    {
        if (FirstAttempt)
            return Reset();
        else {
            QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось открыть файл конфига");
            return false;
        }
    };

    if (file.open(QIODevice::ReadOnly))
    {
        QString errorStr;
        int errorLine;
        int errorColumn;
        if (domDoc.setContent(&file, true, &errorStr, &errorLine,
                              &errorColumn))
        {
            ParsingStats stats;
            InitializeAssParsing(&stats,domDoc);
            InitializeSrtParsing(&stats,domDoc);
            InitializeTimingParsing(&stats,domDoc);
            InitializeStyleParsing(&stats,domDoc);
            InitializeAdditionalParsing(&stats,domDoc);
            if (stats.Count != 0)
            {
                if (!stats.Unrecognized.empty())
                {
                    std::stringstream ss;
                    ss << "В ходе открытия файла конфига(" SETTINGS_DIR ") произошли некоторые ошибки, возможно этот конфликт был спровоцирован ручным вмешательством в этот файл со стороны пользователя." << std::endl;
                    ss << "Следующие значения не были распознаны и оставлены в нулевом состоянии:" << std::endl;
                    for (const QString& Value : stats.Unrecognized)
                    {
                        ss << "\t\t" << PrintConfigPath(Value).toUtf8().data() << std::endl;
                    }
                    ss << "Вам следует самим проставить эти значения, или вернуть опции к состоянию по умолчанию" << std::endl;
                    QMessageBox::warning(QApplication::activeWindow(), "Обратите внимание..", ss.str().c_str());
                }
                return true;
            }
        }
        else {
            qWarning("Line %d, column %d: %s", errorLine, errorColumn,
                         errorStr.toUtf8().data());
        }
    }
    return HandleError();
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


void Settings::InitializeAssParsing(void* rawStats, const QDomDocument &domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    AssParsingInitializeNeededColumns(stats, domDoc, ui->lineEdit_ColumnStart, "Root.AssParsing.NeededColumns.TimeStartColumn");
    AssParsingInitializeNeededColumns(stats, domDoc, ui->lineEdit_ColumnEnd, "Root.AssParsing.NeededColumns.TimeEndColumn");
    AssParsingInitializeNeededColumns(stats, domDoc, ui->lineEdit_ColumnName, "Root.AssParsing.NeededColumns.NameColumn");
    AssParsingInitializeNeededColumns(stats, domDoc, ui->lineEdit_ColumnText, "Root.AssParsing.NeededColumns.TextColumn");
}

void Settings::InitializeSrtParsing(void* rawStats, const QDomDocument & domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    SrtParsingInitializeTimeDistributing(stats, domDoc, ui->comboBox_Distribution, "Root.SrtParsing.ComplexPhrases.TimeDistributing.Strategy");
}

void Settings::InitializeTimingParsing(void* rawStats, const QDomDocument &domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    InitializeSpinBox(stats, domDoc, ui->spinBox_RoundValue, "Root.Timing.MilisecondRoundValue");
    InitializeCheckBox(stats, domDoc, ui->checkBox_DisableIntervals, "Root.Timing.DisableIntervals");
    InitializeSpinBox(stats, domDoc, ui->spinBox_SmallInterval, "Root.Timing.IntervalsDistributing.VerySmall");
    InitializeSpinBox(stats, domDoc, ui->spinBox_NormalInterval, "Root.Timing.IntervalsDistributing.Normal");
    InitializeSpinBox(stats, domDoc, ui->spinBox_BigInterval, "Root.Timing.IntervalsDistributing.Big");
    InitializeSpinBox(stats, domDoc, ui->spinBox_VeryBigInterval, "Root.Timing.IntervalsDistributing.VeryBig");
    InitializeSpinBox(stats, domDoc, ui->spinBox_VeryVeryBigInterval, "Root.Timing.IntervalsDistributing.VeryVeryBig");
}

void Settings::InitializeStyleParsing(void* rawStats, const QDomDocument & domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    InitializeCheckBox(stats, domDoc, ui->checkBox_DisableTags, "Root.DocumentStyle.DisableTags");
    InitializeCheckBox(stats, domDoc, ui->checkBox_DisableCounter, "Root.DocumentStyle.DisableLinesCounter");

     InitializeStyle1Parsing(stats, domDoc);
     InitializeStyle2Parsing(stats, domDoc);
     InitializeStyle3Parsing(stats, domDoc);
}

void Settings::InitializeStyle1Parsing(void* rawStats, const QDomDocument &domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    InitializeLineEdit(stats, domDoc, ui->lineEdit_Font, "Root.DocumentStyle.MainFont.Name");
    InitializeDoubleSpinBox(stats, domDoc, ui->doubleSpinBox_Size, "Root.DocumentStyle.MainFont.Size");
}

void Settings::InitializeStyle2Parsing(void* rawStats, const QDomDocument &domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    InitializeLineEdit(stats, domDoc, ui->lineEdit_Font_2, "Root.DocumentStyle.PageNumberFont.Name");
    InitializeDoubleSpinBox(stats, domDoc, ui->doubleSpinBox_Size_2, "Root.DocumentStyle.PageNumberFont.Size");
}

void Settings::InitializeStyle3Parsing(void* rawStats, const QDomDocument &domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    InitializeLineEdit(stats, domDoc, ui->lineEdit_Font_3, "Root.DocumentStyle.IndividualSelectedFont.Name");
    InitializeDoubleSpinBox(stats, domDoc, ui->doubleSpinBox_Size_3, "Root.DocumentStyle.IndividualSelectedFont.Size");
}

void Settings::InitializeAdditionalParsing(void* rawStats, const QDomDocument &domDoc)
{
    auto stats = reinterpret_cast<ParsingStats*>(rawStats);
    Q_ASSERT(stats);
    InitializeCheckBox(stats, domDoc, ui->checkBox_ExtSound, "Root.Additional.EnableSoundTheme");
}

void Settings::on_CommitedChanges()
{
    ui->pushButton_OK->setEnabled(false);
    ui->pushButton_Apply->setEnabled(false);
    ui->pushButton_Cancel->setFocus();
}

void Settings::on_pushButton_Cancel_clicked()
{
    close();
}

void Settings::on_pushButton_SetDefault_clicked()
{
    Reset();
    this->on_CommitedChanges();
}

void Settings::on_pushButton_OK_clicked()
{
    commitChanges();
    close();
}

void Settings::on_pushButton_Apply_clicked()
{
    commitChanges();
}

void Settings::on_spinBox_RoundValue_valueChanged(int )
{
    beginChanges();
}

void Settings::on_spinBox_SmallInterval_valueChanged(int )
{
    beginChanges();
}

void Settings::on_spinBox_NormalInterval_valueChanged(int )
{
    beginChanges();
}

void Settings::on_spinBox_BigInterval_valueChanged(int )
{
    beginChanges();
}

void Settings::on_spinBox_VeryBigInterval_valueChanged(int )
{
    beginChanges();
}

void Settings::on_spinBox_VeryVeryBigInterval_valueChanged(int )
{
    beginChanges();
}

void Settings::on_lineEdit_Font_textChanged(const QString &)
{
    beginChanges();
}

void Settings::on_doubleSpinBox_Size_valueChanged(double )
{
    beginChanges();
}

void Settings::on_lineEdit_Font_2_textChanged(const QString &)
{
    beginChanges();
}

void Settings::on_doubleSpinBox_Size_2_valueChanged(double )
{
    beginChanges();
}

void Settings::on_lineEdit_Font_3_textChanged(const QString &)
{
    beginChanges();
}

void Settings::on_doubleSpinBox_Size_3_valueChanged(double )
{
    beginChanges();
}

void Settings::on_checkBox_DisableTags_stateChanged(int )
{
    beginChanges();
}

void Settings::on_checkBox_DisableCounter_stateChanged(int )
{
    beginChanges();
}

void Settings::on_checkBox_ExtSound_stateChanged(int )
{
    beginChanges();
}

void Settings::on_comboBox_Distribution_currentIndexChanged(int )
{
    beginChanges();
}

void Settings::on_lineEdit_ColumnText_textChanged(const QString &)
{
    beginChanges();
}

void Settings::on_lineEdit_ColumnStart_textChanged(const QString &)
{
    beginChanges();
}

void Settings::on_lineEdit_ColumnEnd_textChanged(const QString &)
{
    beginChanges();
}

void Settings::on_lineEdit_ColumnName_textChanged(const QString &)
{
    beginChanges();
}
