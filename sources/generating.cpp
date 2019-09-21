#include "generating.h"
#include "ui_generating.h"
#include <QProcess>
#include <sstream>

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

void Generating::StartProcess(const QString &InFile, const QString& ConfigFile, const QString &OutDir)
{
    // TODO: make normal command line
    // TODO: потестить на путях с пробелами и кириллицей
    // TODO: темповому файлу ConfigFile нужна имитозащита
    QStringList Arguments;
    Arguments.reserve(4);
    Arguments.push_back("-make_docx");
    Arguments.push_back(InFile);
    Arguments.push_back(ConfigFile);
    Arguments.push_back(OutDir);
    m_Process->start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}

void Generating::ReadLines()
{
    while (m_Process->canReadLine())
    {
        QString Value = QString::fromUtf8(m_Process->readLine());
        Value.remove("\r\n");
        Value.remove('\r');
        Value.remove('\n');
        ui->plainTextEdit_Log->appendPlainText(Value);
    }
}

void Generating::slotDataOnStdout()
{
    m_Process->setReadChannel(QProcess::StandardOutput);
    ReadLines();
}

void Generating::slotDataOnStderr()
{
    m_Process->setReadChannel(QProcess::StandardError);
    ReadLines();
}

void Generating::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    m_Process->setReadChannel(QProcess::StandardOutput);
    ReadLines();
    m_Process->setReadChannel(QProcess::StandardError);
    ReadLines();

    if (exitCode==0)
    {
        ui->progressBar->setValue(100);
        ui->progressBar->setEnabled(false);
        ui->pushButton_OK->setEnabled(true);
        ui->pushButton_Cancel->setEnabled(false);
        ui->plainTextEdit_Log->appendPlainText("Создание монтажных листов успешно завершено\n");
        ui->label_Status->setText("Успешно");
    }
    else {
        ui->progressBar->setValue(0);
        ui->progressBar->setEnabled(false);
        ui->pushButton_OK->setEnabled(true);
        ui->pushButton_Cancel->setEnabled(false);
        std::stringstream ss;
        ss << "Создание монтажных листов было завершено с ошибкой (код ошибки: " << exitCode << ")\n";
        ui->plainTextEdit_Log->appendPlainText(ss.str().c_str());
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
