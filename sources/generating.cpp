#include "generating.h"
#include "ui_generating.h"
#include <QProcess>
#include <sstream>
#include <ctime>
#include "CommandLineParser.h"
#include <QDebug>
#include <QMessageBox>

#define MAX_PATH 260

Generating::Generating(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Generating)
{
    ui->setupUi(this);

    m_Process     = new QProcess(this);
    connect(m_Process,
                    SIGNAL(readyReadStandardOutput()),
                    SLOT(slotDataOnStdout())
                   );
    connect(m_Process,
                    SIGNAL(readyReadStandardError()),
                    SLOT(slotDataOnStderr())
                   );
    connect(m_Process,
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
}

Generating::~Generating()
{
    delete ui;
}

void Generating::StartProcess(const QString &InFile, const QString& TableFile, const QString &OutDir, const QString& CRC32_TableFile)
{
    // TODO: make normal command line
    // TODO: потестить на путях с пробелами и кириллицей
    // TODO: темповому файлу ConfigFile нужна имитозащита
    QStringList Arguments;
    Arguments.reserve(5);
    Arguments.push_back("-make_docx");
    Arguments.push_back(InFile);
    Arguments.push_back(TableFile);
    Arguments.push_back(OutDir);
    Arguments.push_back(CRC32_TableFile);
    m_Process->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}

void Generating::StdoutReadLines()
{
    m_Process->setReadChannel(QProcess::StandardOutput);
    while (m_Process->canReadLine())
    {
        QString Value = QString::fromUtf8(m_Process->readLine());
        Value.remove("\r\n");
        Value.remove('\r');
        Value.remove('\n');
        CommandLineParser P(Value);
        HandleCommandFromConverter(P.argc(), P.argv());

    }
}

void Generating::StderrReadLines()
{
    m_Process->setReadChannel(QProcess::StandardError);
    while (m_Process->canReadLine())
    {
        QString Value = QString::fromUtf8(m_Process->readLine());
        Value.remove("\r\n");
        Value.remove('\r');
        Value.remove('\n');
        ui->listWidget_Log->addItem(Value);
    }
}

void Generating::HandleCommandFromConverter(int argc, char **argv)
{
    if (argc > 0)
    {
        if (strcmp(argv[0], "start_generating")==0)
        {
            if (argc > 1)
            {
                char* dummyEnd;
                m_MaxDocumentsCount = std::strtoul(argv[1], &dummyEnd, 10);
            }
        }
        else if (strcmp(argv[0], "make_docx_begin")==0)
        {
            if (argc > 1)
            {
                char Buffer[MAX_PATH+64] = {0};
                strcat_s(Buffer, sizeof(Buffer), "----------");
                strcat_s(Buffer, sizeof(Buffer), argv[1]);
                //strcat_s(Buffer, sizeof(Buffer), "----------");
                ui->listWidget_Log->addItem(Buffer);
            }
            else {
                char Buffer[MAX_PATH+64] = {0};
                strcat_s(Buffer, sizeof(Buffer), "----------");
                //strcat_s(Buffer, sizeof(Buffer), "----------");
                ui->listWidget_Log->addItem(Buffer);
            }

            if (argc > 1)
            {
                char Buffer[MAX_PATH+64] = {0};
                strcat_s(Buffer, sizeof(Buffer), "--:--:-- * ");
                strcat_s(Buffer, sizeof(Buffer), argv[1]);
                int DocumentID = ui->listWidget_Stages->count();
                ui->listWidget_Stages->addItem(Buffer);
                m_DomentIds[argv[1]] = DocumentID;
            }
        }
        else if (strcmp(argv[0], "make_docx_end")==0)
        {
            if (argc > 1)
            {
                if (m_MaxDocumentsCount==0)
                    ui->progressBar->setValue(0);
                else
                {
                    m_CurDocumentCount++;
                    ui->progressBar->setValue(
                                static_cast<int>(
                                    100.0 * (m_CurDocumentCount / static_cast<double>(m_MaxDocumentsCount))));
                }
                auto it = m_DomentIds.find(argv[1]);
                if (it != m_DomentIds.end())
                {
                    char Buffer[MAX_PATH+64] = {0};
                    struct tm newtime;
                    __time64_t long_time;
                    errno_t err;
                    _time64( &long_time );
                    err = _localtime64_s( &newtime, &long_time );
                    if (!err)
                    {
                        sprintf_s(Buffer, sizeof(Buffer), "%02d:%02d:%02d : ", newtime.tm_hour, newtime.tm_min, newtime.tm_sec);
                        strcat_s(Buffer, sizeof(Buffer), argv[1]);
                        auto item = ui->listWidget_Stages->item(it.value());
                        if (item)
                            item->setText(Buffer);
                    }
                }
            }
        }
        else if (strcmp(argv[0], "info")==0)
        {
            for (int i=1;i<argc;++i)
                ui->listWidget_Log->addItem(argv[i]);
        }
    }
}

void Generating::slotDataOnStdout()
{
    StdoutReadLines();
}

void Generating::slotDataOnStderr()
{
    StderrReadLines();
}

void Generating::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    StdoutReadLines();
    StderrReadLines();

    if (exitCode==0)
    {
        ui->progressBar->setValue(100);
        ui->progressBar->setEnabled(false);
        ui->pushButton_OK->setEnabled(true);
        ui->pushButton_Cancel->setEnabled(false);
        ui->listWidget_Log->addItem("Создание монтажных листов успешно завершено\n");
        ui->label_Status->setText("Успешно");
    }
    else {
        ui->progressBar->setValue(0);
        ui->progressBar->setEnabled(false);
        ui->pushButton_OK->setEnabled(true);
        ui->pushButton_Cancel->setEnabled(false);
        std::stringstream ss;
        ss << "Создание монтажных листов было завершено с ошибкой (код ошибки: " << exitCode << ")\n";
        ui->listWidget_Log->addItem(ss.str().c_str());
        ui->label_Status->setText("Что-то пошло не так..");
    }
}

void Generating::on_pushButton_OK_clicked()
{
    close();
}

void Generating::on_pushButton_Cancel_clicked()
{
    close();
}
