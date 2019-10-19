#include "settings.h"
#include "ui_settings.h"
#include <QtXml>
#include <QDebug>
#include <QMessageBox>
#include <sstream>
#include "converterwaiting.h"
#include "xmlsmartkeysprovider.h"

#define SETTINGS_DIR "settings.xml"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    InitializeMainTable();

    // Всегда начинаем с первой страницы..
    // Не важно, что у нас там в редакторе осталось
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
}

Settings::~Settings()
{
    delete ui;
}

bool Settings::Initialize()
{
    bool Status = _Initialize(true);
    // ui->pushButton_OK->setEnabled(false);
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
    //ui->pushButton_OK->setEnabled(true);
    ui->pushButton_Apply->setEnabled(true);
    //ui->pushButton_Apply->setFocus();
}

bool Settings::commitChanges()
{
    QFile file(SETTINGS_DIR);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream streamFileOut(&file);
        streamFileOut.setCodec("UTF-8");
        // для xml не нужен BOM
        //streamFileOut.setGenerateByteOrderMark(true);
        {
            QDomDocument xmlDoc;
            xmlDoc.setContent(QString() + "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
            m_Table.Write(xmlDoc);
            streamFileOut << xmlDoc.toString();
        } streamFileOut.flush();
        file.close();
        this->on_CommitedChanges();
        return true;
    }
    return false;
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
            SettingsParsingStats stats;
            m_Table.Read(&stats, domDoc);
            if (stats.Count != 0)
            {
                if (!stats.Unrecognized.empty())
                {
                    std::stringstream ss;
                    ss << "В ходе открытия файла конфига(" SETTINGS_DIR ") произошли некоторые ошибки, возможно этот конфликт был спровоцирован ручным вмешательством в этот файл со стороны пользователя." << std::endl;
                    ss << "Следующие значения не были распознаны и оставлены в нулевом состоянии:" << std::endl;
                    for (const QString& Value : stats.Unrecognized)
                    {
                        ss << "\t\t" << m_Table.GetFullNameByPath(Value).toUtf8().data() << std::endl;
                    }
                    ss << "Вам следует самим проставить эти значения, или вернуть опции к состоянию по умолчанию" << std::endl;
                    QMessageBox::warning(this, "Обратите внимание..", ss.str().c_str());
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
static
QString TextGetter(QWidget* widget)
{
    T* casted = qobject_cast<T*>(widget);
    Q_ASSERT(casted);
    return casted->text();
}

static
bool AssNeededColumnsReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QString text;
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
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
            return true;
        }
    }
    return false;
}

static
void AssNeededColumnsWriter(QWidget* widget,QStringList& outLines)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
    Q_ASSERT(lineEdit);

    const QString& text = lineEdit->text();
    QStringList sections = text.split(",", QString::SkipEmptyParts);
    for (int i=0;i<sections.size();++i)
    {
        sections[i] = sections[i].trimmed();
    }
    std::swap(outLines, sections);
}


static
bool SrtTimeDistributingReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
    Q_ASSERT(comboBox);

    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText == "Uniform")
        {
            comboBox->setCurrentIndex(0);
            return true;
        }
        else if (nodeText == "Fair")
        {
            comboBox->setCurrentIndex(1);
            return true;
        }
    }
    return false;
}

static
void SrtTimeDistributingWriter(QWidget* widget,QStringList& outLines)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
    Q_ASSERT(comboBox);

    outLines.push_back(comboBox->currentText());
}

static
bool PackingMethodReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
    Q_ASSERT(comboBox);

    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText == "disable")
        {
            comboBox->setCurrentIndex(0);
            return true;
        }
        else if (nodeText == "enable")
        {
            comboBox->setCurrentIndex(1);
            return true;
        }
        else if (nodeText == "enable_intervals")
        {
            comboBox->setCurrentIndex(2);
            return true;
        }
    }
    return false;
}

static
void PackingMethodWriter(QWidget* widget,QStringList& outLines)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
    Q_ASSERT(comboBox);

    switch (comboBox->currentIndex())
    {
    case 0:
        outLines.push_back("disable");
        break;
    case 1:
        outLines.push_back("enable");
        break;
    case 2:
        outLines.push_back("enable_intervals");
        break;
    default:
        Q_ASSERT_X(0, __FUNCTION__,  "Invalid index");
        break;
    }
}

static
bool CheckBoxReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
    Q_ASSERT(checkBox);

    if (nodes.size() == 1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText=="true")
        {
            checkBox->setCheckState(Qt::CheckState::Checked);
            return true;
        }
        else if (nodeText=="false")
        {
            checkBox->setCheckState(Qt::CheckState::Unchecked);
            return true;
        }
    }
    return false;
}

static
void CheckBoxWriter(QWidget* widget,QStringList& outLines)
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
    Q_ASSERT(checkBox);

    switch (checkBox->checkState())
    {
    case Qt::CheckState::Checked:
        outLines.push_back("true");
        break;
    case Qt::CheckState::Unchecked:
        outLines.push_back("false");
        break;
    default:
        Q_ASSERT_X(0, __FUNCTION__,  "Invalid 'Qt::CheckState'");
        break;
    }
}

static
bool SpinBoxReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget);
    Q_ASSERT(spinBox);

    if (nodes.size() == 1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        int iValue = 0;
        bool castStatus = false;
        iValue = nodeText.toInt(&castStatus);
        if (castStatus)
        {
            spinBox->setValue(iValue);
            return true;
        }
    }
    return false;
}

// for QSpinBox, QLineEdit
template <typename T>
static
void AbstractWriter(QWidget* widget,QStringList& outLines)
{
    T* concrete = qobject_cast<T*>(widget);
    Q_ASSERT(concrete);

    outLines.push_back(concrete->text());
}

static
bool SpinBoxDoubleReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget);
    Q_ASSERT(spinBox);
    if (nodes.size() == 1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        double dValue = 0;
        bool castStatus = false;
        dValue = nodeText.toDouble(&castStatus);
        if (castStatus)
        {
            spinBox->setValue(dValue);
            return true;
        }
    }
    return false;
}

static
void SpinBoxDoubleWriter(QWidget* widget,QStringList& outLines)
{
    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget);
    Q_ASSERT(spinBox);

    outLines.push_back(QString::number(spinBox->value()));
}

static
bool LineEditReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
    Q_ASSERT(lineEdit);

    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText.size()!=0)
        {
            lineEdit->setText(nodeText);
            return true;
        }
    }
    return false;
}

template <typename T>
static
bool ColorableReader(QWidget* widget, const QList<QDomNode>& nodes)
{
    T* editor = qobject_cast<T*>(widget);
    Q_ASSERT(editor);

    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (QColor::isValidColor(nodeText))
        {
            editor->setColor(QColor(nodeText));
            return true;
        }
    }
    return false;
}

template <typename T>
static
void ColorableWriter(QWidget* widget,QStringList& outLines)
{
    T* editor = qobject_cast<T*>(widget);
    Q_ASSERT(editor);

    outLines.push_back(editor->color().name());
}

void Settings::InitializeMainTable()
{
    const QString ASS_CATEGORY = "ASS";
    const QString SRT_CATEGORY = "SRT";
    const QString TIMING_CATEGORY = "Тайминг";
    const QString STYLE_CATEGORY = "Стиль документа";
    const QString STYLE_CATEGORY_MAIN = "Стиль документа:Главное";
    const QString STYLE_CATEGORY_NUMERATION = "Стиль документа:Нумерация";
    const QString STYLE_CATEGORY_SELECTION = "Стиль документа:Выделенные фразы в индивидуальном документе";
    const QString STYLE_ADDITIONAL = "Дополнительно";

#define REG(...) m_Table.Register(__VA_ARGS__)

    /*******************************************************************
    * AssParsing
    *******************************************************************/
    REG("Root.AssParsing.NeededColumns.TimeStartColumn",
        ASS_CATEGORY, ui->label_ColumnStart,
        ui->lineEdit_ColumnStart, AssNeededColumnsReader, AssNeededColumnsWriter, nullptr);
    connect(ui->lineEdit_ColumnStart,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.AssParsing.NeededColumns.TimeEndColumn",
        ASS_CATEGORY, ui->label_ColumnEnd,
        ui->lineEdit_ColumnEnd, AssNeededColumnsReader, AssNeededColumnsWriter, nullptr);
    connect(ui->lineEdit_ColumnEnd,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.AssParsing.NeededColumns.NameColumn",
        ASS_CATEGORY, ui->label_ColumnName,
        ui->lineEdit_ColumnName, AssNeededColumnsReader, AssNeededColumnsWriter, nullptr);
    connect(ui->lineEdit_ColumnName,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.AssParsing.NeededColumns.TextColumn",
        ASS_CATEGORY, ui->label_ColumnText,
        ui->lineEdit_ColumnText, AssNeededColumnsReader, AssNeededColumnsWriter, nullptr);
    connect(ui->lineEdit_ColumnText,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    /*******************************************************************
    * SrtParsing
    *******************************************************************/
    REG("Root.SrtParsing.ComplexPhrases.TimeDistributing.Strategy",
        SRT_CATEGORY, ui->label_Distribution,
        ui->comboBox_Distribution, SrtTimeDistributingReader, SrtTimeDistributingWriter, nullptr);
    connect(ui->comboBox_Distribution,
                    &QComboBox::currentTextChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    /*******************************************************************
    * Timing
    *******************************************************************/
    REG("Root.Timing.MilisecondRoundValue",
        TIMING_CATEGORY, ui->label_RoundValue,
        ui->spinBox_RoundValue, SpinBoxReader, AbstractWriter<QSpinBox>, nullptr);
    // TODO: test it
    connect(ui->spinBox_RoundValue,
                    static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.Timing.PackingMethod",
        TIMING_CATEGORY, ui->label_PackingMethod,
        ui->comboBox_PackingMethod, PackingMethodReader, PackingMethodWriter, nullptr);
    connect(ui->comboBox_PackingMethod,
                    &QComboBox::currentTextChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.Timing.IntervalsDistributing.VerySmall",
        TIMING_CATEGORY, ui->label_SmallInterval,
        ui->spinBox_SmallInterval, SpinBoxReader, AbstractWriter<QSpinBox>, nullptr);
    connect(ui->spinBox_SmallInterval,
                    static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.Timing.IntervalsDistributing.Normal",
        TIMING_CATEGORY, ui->label_NormalInterval,
        ui->spinBox_NormalInterval, SpinBoxReader, AbstractWriter<QSpinBox>, nullptr);
    connect(ui->spinBox_NormalInterval,
                    static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );


    REG("Root.Timing.IntervalsDistributing.Big",
        TIMING_CATEGORY, ui->label_BigInterval,
        ui->spinBox_BigInterval, SpinBoxReader, AbstractWriter<QSpinBox>, nullptr);
    connect(ui->spinBox_BigInterval,
                    static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.Timing.IntervalsDistributing.VeryBig",
        TIMING_CATEGORY, ui->label_VeryBigInterval,
        ui->spinBox_VeryBigInterval, SpinBoxReader, AbstractWriter<QSpinBox>, nullptr);
    connect(ui->spinBox_VeryBigInterval,
                    static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.Timing.IntervalsDistributing.VeryVeryBig",
        TIMING_CATEGORY, ui->label_VeryVeryBigInterval,
        ui->spinBox_VeryVeryBigInterval, SpinBoxReader, AbstractWriter<QSpinBox>, nullptr);
    connect(ui->spinBox_VeryVeryBigInterval,
                    static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    /*******************************************************************
    * DocumentStyle
    *******************************************************************/
    REG("Root.DocumentStyle.DisableTags",
        STYLE_CATEGORY, nullptr,
        ui->checkBox_DisableTags, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_DisableTags,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.DisableLinesCounter",
        STYLE_CATEGORY, nullptr,
        ui->checkBox_DisableCounter, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_DisableCounter,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.Name",
        STYLE_CATEGORY_MAIN, ui->label_StyleFont,
        ui->lineEdit_StyleFont, LineEditReader, AbstractWriter<QLineEdit>, nullptr);
    connect(ui->lineEdit_StyleFont,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.Size",
        STYLE_CATEGORY_MAIN, ui->label_StyleSize,
        ui->doubleSpinBox_StyleSize, SpinBoxDoubleReader, SpinBoxDoubleWriter, nullptr);
    connect(ui->doubleSpinBox_StyleSize,
                    static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.Color",
        STYLE_CATEGORY_MAIN, ui->label_StyleColor,
        ui->coloredPushButton_StyleColor, ColorableReader<ColoredPushButton>, ColorableWriter<ColoredPushButton>, nullptr);
    connect(ui->coloredPushButton_StyleColor,
                    &ColoredPushButton::colorChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.ColorBack",
        STYLE_CATEGORY_MAIN, ui->label_StyleColorBack,
        ui->comboBox_StyleColorBack, ColorableReader<ColorListEditor>, ColorableWriter<ColorListEditor>, nullptr);
    connect(ui->comboBox_StyleColorBack,
                    &QComboBox::currentTextChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.Bold",
        STYLE_CATEGORY_MAIN, nullptr,
        ui->checkBox_StyleBold, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleBold,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.Italic",
        STYLE_CATEGORY_MAIN, nullptr,
        ui->checkBox_StyleItalic, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleItalic,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.MainFont.Underline",
        STYLE_CATEGORY_MAIN, nullptr,
        ui->checkBox_StyleUnderline, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleUnderline,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );


    REG("Root.DocumentStyle.PageNumberFont.Name",
        STYLE_CATEGORY_NUMERATION, ui->label_StyleFont_2,
        ui->lineEdit_StyleFont_2, LineEditReader, AbstractWriter<QLineEdit>, nullptr);
    connect(ui->lineEdit_StyleFont_2,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.PageNumberFont.Size",
        STYLE_CATEGORY_NUMERATION, ui->label_StyleSize_2,
        ui->doubleSpinBox_StyleSize_2, SpinBoxDoubleReader, SpinBoxDoubleWriter, nullptr);
    connect(ui->doubleSpinBox_StyleSize_2,
                    static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.PageNumberFont.Color",
        STYLE_CATEGORY_NUMERATION, ui->label_StyleColor_2,
        ui->coloredPushButton_StyleColor_2, ColorableReader<ColoredPushButton>, ColorableWriter<ColoredPushButton>, nullptr);
    connect(ui->coloredPushButton_StyleColor_2,
                    &ColoredPushButton::colorChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.PageNumberFont.ColorBack",
        STYLE_CATEGORY_NUMERATION, ui->label_StyleColorBack_2,
        ui->comboBox_StyleColorBack_2, ColorableReader<ColorListEditor>, ColorableWriter<ColorListEditor>, nullptr);
    connect(ui->comboBox_StyleColorBack_2,
                    &QComboBox::currentTextChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.PageNumberFont.Bold",
        STYLE_CATEGORY_NUMERATION, nullptr,
        ui->checkBox_StyleBold_2, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleBold_2,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.PageNumberFont.Italic",
        STYLE_CATEGORY_NUMERATION, nullptr,
        ui->checkBox_StyleItalic_2, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleItalic_2,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.PageNumberFont.Underline",
        STYLE_CATEGORY_NUMERATION, nullptr,
        ui->checkBox_StyleUnderline_2, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleUnderline_2,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.Name",
        STYLE_CATEGORY_SELECTION, ui->label_StyleFont_3,
        ui->lineEdit_StyleFont_3, LineEditReader, AbstractWriter<QLineEdit>, nullptr);
    connect(ui->lineEdit_StyleFont_3,
                    &QLineEdit::textChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.Size",
        STYLE_CATEGORY_SELECTION, ui->label_StyleSize_3,
        ui->doubleSpinBox_StyleSize_3, SpinBoxDoubleReader, SpinBoxDoubleWriter, nullptr);
    connect(ui->doubleSpinBox_StyleSize_3,
                    static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.Color",
        STYLE_CATEGORY_SELECTION, ui->label_StyleColor_3,
        ui->coloredPushButton_StyleColor_3, ColorableReader<ColoredPushButton>, ColorableWriter<ColoredPushButton>, nullptr);
    connect(ui->coloredPushButton_StyleColor_3,
                    &ColoredPushButton::colorChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.ColorBack",
        STYLE_CATEGORY_SELECTION, ui->label_StyleColorBack_3,
        ui->comboBox_StyleColorBack_3, ColorableReader<ColorListEditor>, ColorableWriter<ColorListEditor>, nullptr);
    connect(ui->comboBox_StyleColorBack_3,
                    &QComboBox::currentTextChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.Bold",
        STYLE_CATEGORY_SELECTION, nullptr,
        ui->checkBox_StyleBold_3, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleBold_3,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.Italic",
        STYLE_CATEGORY_SELECTION, nullptr,
        ui->checkBox_StyleItalic_3, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleItalic_3,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

    REG("Root.DocumentStyle.IndividualSelectedFont.Underline",
        STYLE_CATEGORY_SELECTION, nullptr,
        ui->checkBox_StyleUnderline_3, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_StyleUnderline_3,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );



    /*******************************************************************
    * Additional
    *******************************************************************/
    REG("Root.Additional.EnableSoundTheme", STYLE_ADDITIONAL, nullptr, ui->checkBox_ExtSound, CheckBoxReader, CheckBoxWriter, TextGetter<QCheckBox>);
    connect(ui->checkBox_ExtSound,
                    &QCheckBox::stateChanged,
                    this,
                    &Settings::on_AnyValueEdited
                   );

#undef REG
}

void Settings::on_CommitedChanges()
{
    //ui->pushButton_OK->setEnabled(false);
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
    if (commitChanges())
        close();
    else {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл конфига на запись");
    }
}

void Settings::on_pushButton_Apply_clicked()
{
    if (commitChanges())
    {
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл конфига на запись");
    }
}

void Settings::on_AnyValueEdited()
{
    //qDebug() << "on_AnyValueEdited";
    beginChanges();
}

void SettingsTable::Register(
        const QString &Path,
        const QString &Category,
        QWidget *Label,
        QWidget *Widget,
        ReaderFuncType ReaderFunc,
        WriterFuncType WriterFunc,
        GetTextFuncType GetTextFunc)
{
    Q_ASSERT(Widget);
    Q_ASSERT(ReaderFunc);
    Q_ASSERT(WriterFunc);

    if (GetTextFunc==nullptr)
    {
        Q_ASSERT(Label);
        GetTextFunc = [](QWidget * widget) -> QString
        {
            auto label = qobject_cast<QLabel*>(widget);
            Q_ASSERT(label);
            return label->text();
        };
    }

    Option opt;
    opt.Path                    = Path;
    opt.CategoryName            = Category;
    opt.Label                   = Label;
    opt.Widget                  = Widget;
    opt.GetTextFunc             = GetTextFunc;
    opt.ReaderFunc              = ReaderFunc;
    opt.WriterFunc              = WriterFunc;
    m_MainTable[Path]           = opt;
}

void SettingsTable::PrintFullTable() const
{
    for (const auto& pair : m_MainTable)
    {
        qDebug() << "[" << pair.first << "] = " << GetFullNameByPath(pair.first);
    }
}

void SettingsTable::Read(SettingsParsingStats *Stats, const QDomDocument &domDoc)
{
    for (const auto& pair : m_MainTable)
    {
        const auto& value = pair.second;
        QList<QDomNode> nodes = XmlSmartKeysProvider::elementsByTagPath(domDoc, value.Path);
        if (value.ReaderFunc(value.Widget, nodes))
            (Stats->Count)++;
        else
        {
            Stats->Unrecognized.push_back(value.Path);
        }
    }
}

void SettingsTable::Write(QDomDocument &writer) const
{
    for (const auto& pair : m_MainTable)
    {
        const auto& value = pair.second;
        QStringList outValues;
        value.WriterFunc(value.Widget, outValues);
        XmlSmartKeysProvider::writeTextElement(writer, value.Path, outValues);
    }
}

QString SettingsTable::GetFullNameByPath(const QString &Path) const
{
    //qDebug() << "GetFullNameByPath: " << Path;
    const auto it = m_MainTable.find(Path);
    if (it != m_MainTable.end())
    {
        const auto& obj  = it->second;
        if (obj.GetTextFunc)
        {
            auto labelWidget = obj.Label;
            if (labelWidget == nullptr)
                labelWidget = obj.Widget;
            return obj.CategoryName + ":" + obj.GetTextFunc(labelWidget) ;
        }
    }
    return "";
}

void Settings::on_comboBox_PackingMethod_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    case 1:
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
    case 2:
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
        Q_ASSERT_X(0, __FUNCTION__,  "Invalid index");
        break;
    }
}
