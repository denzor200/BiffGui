#include "auth.h"
#include "ui_auth.h"
#include "registration.h"
#include <QFileDialog>
#include "converterwaiting.h"
#include <QMessageBox>
#include <QDebug>

Auth::Auth(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Auth)
{
    ui->setupUi(this);
}

Auth::~Auth()
{
    delete ui;
}

void Auth::on_pushButton_makeRequest_clicked()
{
    Registration w;
    w.exec();
}

void Auth::on_commandLinkButton_Continue_clicked()
{
    QString certFilename = ui->lineEdit_CertFile->text();
    if (certFilename != "")
    {
        // копируем файл сертификата..
        if (QFile::exists("user.crt"))
        {
            QFile::remove("user.crt");
        }
        QFile::copy(certFilename, "user.crt");

        // вызываем дочерний процесс чтобы проверить операцию на успешность
        ConverterWaiting_VerifyCert waiting;
        waiting.StartProcess();
        int execStatus = waiting.exec();
        bool isCanceled = waiting.IsCanceledByUser();
        int procStatus = waiting.GetProcessStatus();
		if (isCanceled)
			return;

        if (0 == execStatus)
        {
            if (0 == procStatus)
            {
                QMessageBox::information(this, "Успешно", "Активация прошла успешно!");
                close();
            }
            // else .. Дочерний процесс должен вывести сообщение
        }
        else {
            QMessageBox::critical(this, "Что-то пошло не так..", "Активация не удалась");
        }
    }
}

void Auth::on_toolButton_CertFile_clicked()
{
    QString certFilename = QFileDialog::getOpenFileName(this,
        tr("Открыть сертификат"),
        "",
        tr("Файлы сертификатов (*.crt)"));
    if (certFilename != "")
    {
        ui->lineEdit_CertFile->setText(certFilename);
    }
}
