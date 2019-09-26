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
    // TODO: бросить исключение, если файл не найден
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
QList<QDomNode> elementsByTagPath(const T& doc, QStringList& names)
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
QList<QDomNode> elementsByTagPath(const T& doc, const QString& Path)
{
    QStringList names = Path.split(".");
    return elementsByTagPath(doc, names);
}

int Settings::InitializeAssParsing(const QDomDocument &domDoc)
{
    // TODO: запретить запятые
    // TODO: предусмотреть случай, если nodes оказался пустой
    int count = 0;
    {

        QString text;
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.AssParsing.NeededColumns.TimeStartColumn");
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
                ui->lineEdit_ColumnStart->setText(text);
                count++;
            }
        }
    }
    {
        QString text;
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.AssParsing.NeededColumns.TimeEndColumn");
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
                ui->lineEdit_ColumnEnd->setText(text);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QString text;
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.AssParsing.NeededColumns.NameColumn");
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
                ui->lineEdit_ColumnName->setText(text);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QString text;
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.AssParsing.NeededColumns.TextColumn");
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
                ui->lineEdit_ColumnText->setText(text);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    return count;
}

int Settings::InitializeSrtParsing(const QDomDocument & domDoc)
{
    int count = 0;
    QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.SrtParsing.ComplexPhrases.TimeDistributing.Strategy");
    if (nodes.size()==1)
    {
        const QString& nodeText = nodes[0].toElement().text();
        if (nodeText == "Uniform")
        {
            ui->comboBox_Distribution->setCurrentIndex(0);
            count++;
        }
        else if (nodeText == "Fair")
        {
            ui->comboBox_Distribution->setCurrentIndex(1);
            count++;
        }
        else {
            // TODO: handle
        }
    }
    else {
        // TODO: handle
    }
    return count;
}

int Settings::InitializeTimingParsing(const QDomDocument &domDoc)
{
    int count = 0;
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.MilisecondRoundValue");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid int conversing
            ui->spinBox_RoundValue->setValue(nodeText.toInt());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.DisableIntervals");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText=="true")
            {
                ui->checkBox_DisableIntervals->setCheckState(Qt::CheckState::Checked);
                count++;
            }
            else if (nodeText=="false")
            {
                ui->checkBox_DisableIntervals->setCheckState(Qt::CheckState::Unchecked);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.IntervalsDistributing.VerySmall");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid int conversing
            ui->spinBox_SmallInterval->setValue(nodeText.toInt());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.IntervalsDistributing.Normal");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid int conversing
            ui->spinBox_NormalInterval->setValue(nodeText.toInt());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.IntervalsDistributing.Big");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid int conversing
            ui->spinBox_BigInterval->setValue(nodeText.toInt());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.IntervalsDistributing.VeryBig");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid int conversing
            ui->spinBox_VeryBigInterval->setValue(nodeText.toInt());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Timing.IntervalsDistributing.VeryVeryBig");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid int conversing
            ui->spinBox_VeryVeryBigInterval->setValue(nodeText.toInt());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    return count;
}

int Settings::InitializeStyleParsing(const QDomDocument & domDoc)
{
    int count = 0;
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.DisableTags");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText=="true")
            {
                ui->checkBox_DisableTags->setCheckState(Qt::CheckState::Checked);
                count++;
            }
            else if (nodeText=="false")
            {
                ui->checkBox_DisableTags->setCheckState(Qt::CheckState::Unchecked);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.DisableLinesCounter");
        if (nodes.size() == 1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText=="true")
            {
                ui->checkBox_DisableCounter->setCheckState(Qt::CheckState::Checked);
                count++;
            }
            else if (nodeText=="false")
            {
                ui->checkBox_DisableCounter->setCheckState(Qt::CheckState::Unchecked);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    count += InitializeStyle1Parsing(domDoc);
    count += InitializeStyle2Parsing(domDoc);
    count += InitializeStyle3Parsing(domDoc);
    return count;
}

int Settings::InitializeStyle1Parsing(const QDomDocument &domDoc)
{
    int count = 0;
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.MainFont.Name");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText.size()!=0)
            {
                ui->lineEdit_Font->setText(nodeText);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.MainFont.Size");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid double conversion
            ui->doubleSpinBox_Size->setValue(nodeText.toDouble());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    return count;
}

int Settings::InitializeStyle2Parsing(const QDomDocument &domDoc)
{
    int count = 0;
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.PageNumberFont.Name");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText.size()!=0)
            {
                ui->lineEdit_Font_2->setText(nodeText);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.PageNumberFont.Size");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid double conversion
            ui->doubleSpinBox_Size_2->setValue(nodeText.toDouble());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    return count;
}

int Settings::InitializeStyle3Parsing(const QDomDocument &domDoc)
{
    int count = 0;
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.IndividualSelectedFont.Name");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText.size()!=0)
            {
                ui->lineEdit_Font_3->setText(nodeText);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.DocumentStyle.IndividualSelectedFont.Size");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            // TODO: handle invalid double conversion
            ui->doubleSpinBox_Size_3->setValue(nodeText.toDouble());
            count++;
        }
        else {
            // TODO: handle
        }
    }
    return count;
}

int Settings::InitializeAdditionalParsing(const QDomDocument &domDoc)
{
    int count = 0;
    {
        QList<QDomNode> nodes = elementsByTagPath(domDoc, "Root.Additional.EnableSoundTheme");
        if (nodes.size()==1)
        {
            const QString& nodeText = nodes[0].toElement().text();
            if (nodeText=="true")
            {
                ui->checkBox_ExtSound->setCheckState(Qt::CheckState::Checked);
                count++;
            }
            else if (nodeText=="false")
            {
                ui->checkBox_ExtSound->setCheckState(Qt::CheckState::Unchecked);
                count++;
            }
            else {
                // TODO: handle
            }
        }
        else {
            // TODO: handle
        }
    }
    return count;
}
