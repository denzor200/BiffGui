#include "generating.h"
#include "ui_generating.h"
#include <QProcess>
#include <sstream>

Generating::Generating(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    m_Process     = new QProcess(this);
    connect(m_Process,
                    SIGNAL(readyReadStandardOutput()),
                    SLOT(slotDataOnStdout())
                   );
    connect(m_Process,
                    SIGNAL(finished(int, QProcess::ExitStatus)),
                    SLOT(slotFinished(int, QProcess::ExitStatus))
                   );
    // TODO: make normal command line
    m_Process->start("D:\\repos\\subtitles\\Debug\\converter D:\\repos\\subtitles\\Debug\\Subtitry4.ass -make_docx");
}

Generating::~Generating()
{
    delete m_Process;
    delete ui;
}

void Generating::slotDataOnStdout()
{
    ui->plainTextEdit_Log->appendPlainText(m_Process->readAllStandardOutput());
}

void Generating::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

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
