#include "registration.h"
#include "ui_registration.h"
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "converterwaiting.h"

static const char* s_Manager = "https://vk.com/che_rskov";
static const char* s_ManagerEmail = "che.rskov@yandex.ru";

Registration::Registration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
    ui->comboBox_Country->addItems(QStringList() << "Россия" << "Украина" << "Белоруссия" << "Эстония");

    ui->label_Desc_2->setText(QString() + "<a href=\"" + s_Manager + "\">" + s_Manager+ "</a>");
    ui->label_Desc_2->setTextFormat(Qt::RichText);
    ui->label_Desc_2->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_Desc_2->setOpenExternalLinks(true);

    ui->label_Desc_3->setText(QString() + "<a href=\"mailto:" + s_ManagerEmail + "\">" + s_ManagerEmail+ "</a>");
    ui->label_Desc_3->setTextFormat(Qt::RichText);
    ui->label_Desc_3->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_Desc_3->setOpenExternalLinks(true);
}

Registration::~Registration()
{
    delete ui;
}

QString Registration::getCountryName() const
{
    switch (ui->comboBox_Country->currentIndex())
    {
    case 0:
        return "RU";
    case 1:
        return "UA";
    case 2:
        return "BY";
    case 3:
        return "EE";
    }
    return "";
}

void Registration::on_pushButton_Continue_clicked()
{
    QString outFileName = QFileDialog::getSaveFileName(this,
        tr("Создать заявку"),
        "",
        "");
    if (outFileName != "")
    {
        OpensslWaiting_Genrsa waiting;
        waiting.StartProcess();
        int execStatus = waiting.exec();
        bool isCanceled = waiting.IsCanceledByUser();
        int procStatus = waiting.GetProcessStatus();
        Q_ASSERT(!isCanceled);

        if (0 == execStatus && 0 == procStatus)
        {
            // Ключ был сгенерирован успешно
            // Можно приступать к созданию заявки
            OpensslWaiting_Req waiting;
            waiting.StartProcess(
                        outFileName,
                        getCountryName(),
                        ui->lineEdit_State->text(),
                        ui->lineEdit_City->text(),
                        ui->lineEdit_Company->text(),
                        ui->lineEdit_Department->text(),
                        ui->lineEdit_Email->text());
            int execStatus = waiting.exec();
            bool isCanceled = waiting.IsCanceledByUser();
            int procStatus = waiting.GetProcessStatus();
            Q_ASSERT(!isCanceled);
            if (0 == execStatus && 0 == procStatus)
            {
                // Заявка создана успешно

                close();
            }
            else {
                QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось создать заявку");
            }
        }
        else {
            QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось сгенерировать закрытый ключ");
        }
    }
}

void Registration::on_pushButton_2_clicked()
{
    close();
}
