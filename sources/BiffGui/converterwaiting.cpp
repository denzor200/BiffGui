#include "commandlineparser.h"
#include "converterwaiting.h"
#include "ui_converterwaiting.h"
#include "utils.h"
#include "srt.h"

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

    this->setWindowFlags (this->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    if (DisableCancel)
        ui->pushButton->hide();

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

void ConverterWaiting::on_pushButton_clicked()
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
    Arguments.reserve(3);
    Arguments.push_back("show_person_list");
    Arguments.push_back(SubbtitlePath);
    Arguments.push_back("");
    getProcess()->start("converter", Arguments);
}

bool ConverterWaiting_ShowPersonList::GetCRC(QString &out) const
{
    if (!m_CRC_Initialized)
        return false;
    out = m_CRC;
    return true;
}

bool ConverterWaiting_ShowPersonList::GetMarkupType(SrtFormat::MarkupType &mt) const
{
    if (!m_MarkupType_Initialized)
        return false;
    mt = m_MarkupType;
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
    if (argc >= 1)
    {
        if (strcmp(argv[0], "person")==0)
        {
            if (argc >= 2)
            {
                m_Persons << argv[1];
            }
        }
        else if (strcmp(argv[0], "input_file_crc")==0)
        {
            if (argc >= 2)
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
        else if (strcmp(argv[0], "users_decision")==0)
        {
            if (argc >= 5)
            {
                const char* phrase = argv[1];
                const char* line = argv[2];
                const char* status = argv[3];
                const char* always = argv[4];
                m_UsersDecisions.push_back(
                            QString()
                            + "\"users_decision\" "
                            + "\"" + phrase+ "\" "
                            + "\"" + line+ "\" "
                            + "\"" + status+ "\" "
                            + "\"" + always+ "\"");
            }
        }
        else if (strcmp(argv[0], "markup_type")==0)
        {
            if (argc >= 2)
            {
                auto type = argv[1];
                auto mt = static_cast<SrtFormat::MarkupType>(Utils::stoi(type, -1));
                if (SrtFormat::CheckMarkupType(mt))
                {
                    m_MarkupType = mt;
                    m_MarkupType_Initialized = true;
                }
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

void ConverterWaiting_SaveSubbtitle::StartProcess(const QString &InFile, const QString &Decisions, const QString &OutFile, const QVector<QPair<QString,QString>>& params)
{
    QStringList Arguments;
    Arguments.reserve(4 + params.size()*2);
    Arguments.push_back("make_subbtitle");
    Arguments.push_back(InFile);
    Arguments.push_back(Decisions);
    Arguments.push_back(OutFile);
    for (const auto& pair : params)
    {
        Arguments.push_back(pair.first);
        Arguments.push_back(pair.second);
    }
    getProcess()->start("converter", Arguments);
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
    getProcess()->start("converter", Arguments);
}

void ConverterWaiting_ResetSetting::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}

ConverterWaiting_VerifyCert::ConverterWaiting_VerifyCert(QWidget *parent) :
    ConverterWaiting(false,parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void ConverterWaiting_VerifyCert::StartProcess(bool silent)
{
    QStringList Arguments;
    Arguments.reserve(2);
    Arguments.push_back("verify_cert");
    if (silent)
    {
        Arguments.push_back("-s");
    }
    getProcess()->start("converter", Arguments);
}

void ConverterWaiting_VerifyCert::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}

OpensslWaiting_Genrsa::OpensslWaiting_Genrsa(QWidget *parent) :
    ConverterWaiting(false,parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void OpensslWaiting_Genrsa::StartProcess()
{
    getProcess()->start("openssl\\bin\\openssl.exe genrsa -out client.key 2048");
}

void OpensslWaiting_Genrsa::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}

OpensslWaiting_Req::OpensslWaiting_Req(QWidget *parent) :
    ConverterWaiting(false,parent)
{
    connect(getProcess(),
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

void OpensslWaiting_Req::StartProcess(
        const QString &FileName,
        const QString &Country,
        const QString &Union,
        const QString &City,
        const QString &Company,
        const QString &Department,
        const QString &Email,
        const QString& CN)
{
    getProcess()->start(QString() + "openssl\\bin\\openssl.exe req -new -key client.key -out \"" + FileName + "\"  -config openssl\\bin\\cnf\\openssl.cnf");
    if( getProcess()->waitForStarted() ) {
        getProcess()->write( Country.toUtf8().data() );
        getProcess()->write( "\n" );
        getProcess()->write( Union.toUtf8().data() );
        getProcess()->write( "\n" );
        getProcess()->write( City.toUtf8().data() );
        getProcess()->write( "\n" );
        getProcess()->write( Company.toUtf8().data() );
        getProcess()->write( "\n" );
        getProcess()->write( Department.toUtf8().data() );
        getProcess()->write( "\n" );
        getProcess()->write( CN.toUtf8().data() );
        getProcess()->write( "\n" );
        getProcess()->write( Email.toUtf8().data() );
        getProcess()->write( "\n" );

        // empty challenge password
        getProcess()->write( "\n" );
        // empty optional company name
        getProcess()->write( "\n" );
    }
}

void OpensslWaiting_Req::slotFinished(int Status, QProcess::ExitStatus)
{
    m_ProcessStatus = Status;
}
