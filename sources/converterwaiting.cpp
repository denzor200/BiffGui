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

ConverterWaiting::ConverterWaiting(QWidget *parent) :
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

    QPushButton *b = new QPushButton(this);
    b->setText("Отмена");
    b->setGeometry(0, lbl->height(), b->width(),b->height());
    this->setFixedSize(lbl->width(), lbl->height()+b->height());
    /*this->move(
        (QApplication::activeWindow()->width() - width()) / 2 + QApplication::activeWindow()->x(),
        (QApplication::activeWindow()->height() - height()) / 2 + QApplication::activeWindow()->y());*/
    this->setWindowTitle(" ");
    setWindowFlags(Qt::Dialog | Qt::Desktop );

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
    close();
    m_CanceledByUser = true;
}

void ConverterWaiting::slotFinished(int, QProcess::ExitStatus)
{
    close();
}

ConverterWaiting_ShowPersonList::ConverterWaiting_ShowPersonList(QStringList& Persons,QWidget *parent) :
    ConverterWaiting(parent),
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
    Arguments.push_back("-show_person_list");
    Arguments.push_back(SubbtitlePath);
    // TODO: make normal path
    getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}

void ConverterWaiting_ShowPersonList::slotDataOnStdout()
{
    while (getProcess()->canReadLine())
    {
        m_Persons << QString::fromUtf8(getProcess()->readLine());
    }
}

void ConverterWaiting_ShowPersonList::slotFinished(int Status, QProcess::ExitStatus)
{
    while (getProcess()->canReadLine())
    {
        m_Persons << QString::fromUtf8(getProcess()->readLine());
    }
    m_ProcessStatus = Status;
}


ConverterWaiting_SaveSubbtitle::ConverterWaiting_SaveSubbtitle(QWidget *parent) :
    ConverterWaiting(parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void ConverterWaiting_SaveSubbtitle::StartProcess(const QString &InFile, const QString &OutFile)
{
    QStringList Arguments;
    QString Suffix = QFileInfo(OutFile).suffix();

    if (Suffix == "ass")
    {
        Arguments.reserve(3);
        Arguments.push_back("-make_ass");
        Arguments.push_back(InFile);
        Arguments.push_back(OutFile);
        // TODO: make normal path
        getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
    }
    else if (Suffix == "srt") {
        Arguments.reserve(3);
        Arguments.push_back("-make_srt");
        Arguments.push_back(InFile);
        Arguments.push_back(OutFile);
        // TODO: make normal path
        getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Расширение выходного файла не опознано");
    }
}

void ConverterWaiting_SaveSubbtitle::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}


ConverterWaiting_SaveMySubbtitle::ConverterWaiting_SaveMySubbtitle(QWidget *parent) :
    ConverterWaiting(parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void ConverterWaiting_SaveMySubbtitle::StartProcess(
        const QString &InFile,
        const QString& SettingsFile,
        const QString &OutFile)
{
    QStringList Arguments;
    QString Suffix = QFileInfo(OutFile).suffix();

    // TODO: Мы должны передавать параметром имитовставку от SettingsFile, поскольку это временный не залоченный файл
    if (Suffix == "ass")
    {
        Arguments.reserve(4);
        Arguments.push_back("-make_my_ass");
        Arguments.push_back(InFile);
        Arguments.push_back(SettingsFile);
        Arguments.push_back(OutFile);
        // TODO: make normal path
        getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
    }
    else if (Suffix == "srt") {
        Arguments.reserve(4);
        Arguments.push_back("-make_my_srt");
        Arguments.push_back(InFile);
        Arguments.push_back(SettingsFile);
        Arguments.push_back(OutFile);
        // TODO: make normal path
        getProcess()->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Расширение выходного файла не опознано");
    }
}

void ConverterWaiting_SaveMySubbtitle::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}


