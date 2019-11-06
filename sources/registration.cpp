#include "registration.h"
#include "ui_registration.h"
#include <QWidget>

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

void Registration::on_pushButton_Continue_clicked()
{
    close();
}

void Registration::on_pushButton_2_clicked()
{
    close();
}
