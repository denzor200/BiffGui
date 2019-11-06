#include "auth.h"
#include "ui_auth.h"
#include "registration.h"
#include <QFileDialog>

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

    }
}

void Auth::on_toolButton_CertFile_clicked()
{
    QString certFilename = QFileDialog::getOpenFileName(this,
        tr("Открыть сертификат"),
        "",
        "");
    if (certFilename != "")
    {
        ui->lineEdit_CertFile->setText(certFilename);
    }
}
