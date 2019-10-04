#include "commandlineparser.h"
#include "converterwaiting.h"
#include "ui_converterwaiting.h"

#include <QLabel>
#include <QMovie>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QFormLayout>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <QFileInfo>

ConverterWaiting::ConverterWaiting(bool DisableCancel,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConverterWaiting)
{
    ui->setupUi(this);

    // TODO: гифка не должна лежать рядом с exe
    QLabel *lbl = new QLabel(this);
    QMovie *movie = new QMovie("ajax-loader.gif");
    lbl->setMovie(movie);
    movie->start();
    lbl->show();

    QPushButton* b = nullptr;
    if (!DisableCancel)
    {
        b = new QPushButton(this);
        b->setText("Отмена");
        b->setGeometry(0, lbl->height(), b->width(),b->height());
        this->setFixedSize(lbl->width(), lbl->height()+b->height());
    }
    else {
        this->setFixedSize(lbl->width(), lbl->height());
    }
    /*this->move(
        (QApplication::activeWindow()->width() - width()) / 2 + QApplication::activeWindow()->x(),
        (QApplication::activeWindow()->height() - height()) / 2 + QApplication::activeWindow()->y());*/
    this->setWindowTitle(" ");
    setWindowFlags(Qt::Dialog | Qt::Desktop );

    if (!DisableCancel)
        connect(b, SIGNAL(clicked(bool)), SLOT(ButtonClicked(bool)) );

    m_Process = new QProcess(this);

    connect(m_Process,
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

ConverterWaiting::~ConverterWaiting()
{
    delete ui;
}

void ConverterWaiting::ButtonClicked(bool)
{
    cancel();
}

void ConverterWaiting::slotFinished(int, QProcess::ExitStatus)
{
    close();
}

void ConverterWaiting::cancel()
{
    close();
    m_CanceledByUser = true;
}

ConverterWaiting_ShowPersonList::ConverterWaiting_ShowPersonList(QStringList& Persons,QWidget *parent) :
    ConverterWaiting(false,parent),
    m_Persons(Persons)
{
    connect(getProcess(),
                    SIGNAL(readyReadStandardOutput()),
                    SLOT(slotDataOnStdout())
                   );
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void ConverterWaiting_ShowPersonList::StartProcess(const QString &SubbtitlePath)
{
    QStringList Arguments;
    Arguments.reserve(2);
    Arguments.push_back("show_person_list");
    Arguments.push_back(SubbtitlePath);
    // TODO: make normal path
    getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}

bool ConverterWaiting_ShowPersonList::GetCRC(QString &out) const
{
    if (!m_CRC_Initialized)
        return false;
    out = m_CRC;
    return true;
}

void ConverterWaiting_ShowPersonList::slotDataOnStdout()
{
    if (!StdoutReadLines())
        cancel();
}

void ConverterWaiting_ShowPersonList::slotFinished(int Status, QProcess::ExitStatus)
{
    if (!StdoutReadLines())
        cancel();
    m_ProcessStatus = Status;
}

bool ConverterWaiting_ShowPersonList::StdoutReadLines()
{
    while (getProcess()->canReadLine())
    {
        QString Value = QString::fromUtf8(getProcess()->readLine());
        Value.remove("\r\n");
        Value.remove('\r');
        Value.remove('\n');
        CommandLineParser P(Value);
        if (!HandleCommandFromConverter(P.argc(), P.argv()))
            return false;
    }
    return true;
}

bool ConverterWaiting_ShowPersonList::HandleCommandFromConverter(int argc, char **argv)
{
    if (argc > 0)
    {
        if (strcmp(argv[0], "person")==0)
        {
            if (argc > 1)
            {
                m_Persons << argv[1];
            }
        }
        else if (strcmp(argv[0], "input_file_crc")==0)
        {
            if (argc > 1)
            {
                if (m_CRC_Initialized)
                {
                    QMessageBox::critical(
                                QApplication::activeWindow(),
                                "Что-то пошло не так..",
                                "От дочернего процесса пришла более чем одна контрольная сумма.\n"
                                "Ожидалась только одна контрольная сумма от входного файла.");
                    return false;
                }
                m_CRC = argv[1];
                m_CRC_Initialized = true;
            }
        }
    }
    return true;
}


ConverterWaiting_SaveSubbtitle::ConverterWaiting_SaveSubbtitle(QWidget *parent) :
    ConverterWaiting(false,parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void ConverterWaiting_SaveSubbtitle::StartProcess(const QString &InFile, const QString &OutFile, const QVector<QPair<QString,QString>>& params)
{
    QStringList Arguments;
    Arguments.reserve(3 + params.size()*2);
    Arguments.push_back("make_subbtitle");
    Arguments.push_back(InFile);
    Arguments.push_back(OutFile);
    for (const auto& pair : params)
    {
        Arguments.push_back(pair.first);
        Arguments.push_back(pair.second);
    }
    // TODO: make normal path
    getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}

void ConverterWaiting_SaveSubbtitle::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}




ConverterWaiting_ResetSetting::ConverterWaiting_ResetSetting(QWidget *parent):
    ConverterWaiting(true,parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void ConverterWaiting_ResetSetting::StartProcess()
{
    QStringList Arguments;
    Arguments.reserve(1);
    Arguments.push_back("reset_settings");
    // TODO: make normal path
    getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}

void ConverterWaiting_ResetSetting::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}
