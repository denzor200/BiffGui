#include "registration.h"
#include "ui_registration.h"
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "converterwaiting.h"

static const char* s_Manager = "https://vk.com/che_rskov";
static const char* s_ManagerEmail = "che.rskov@yandex.ru";

extern int getSerialNumber(uint32_t* pVSNumber);

Registration::Registration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
    ui->comboBox_Country->addItems(QStringList() << "Russia" << "Ukraine" << "Belarus" << "Estonia");

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

bool Registration::validateString(const QString &value)
{
    for (char c : value.toUtf8())
    {
        if (c <= 31)
            return false;
        if (c == 0x7f)
            return false;
    }
    return true;
}

void Registration::on_pushButton_Continue_clicked()
{
    const QString& stateText = ui->lineEdit_State->text();
    const QString& cityText = ui->lineEdit_City->text();
    const QString& companyText = ui->lineEdit_Company->text();
    const QString& departmentText = ui->lineEdit_Department->text();
    const QString& emailText = ui->lineEdit_Email->text();

    if (!validateString(stateText)) {
        showError("Регион");
        return;
    }
    if (!validateString(cityText)) {
        showError("Город");
        return;
    }
    if (!validateString(companyText)) {
        showError("Организация");
        return ;
    }
    if (!validateString(departmentText)) {
        showError("Отдел");
        return;
    }
    if (!validateString(emailText)) {
        showError("Email");
        return;
    }

    QString outFileName = QFileDialog::getSaveFileName(this,
        tr("Создать заявку"),
        "",
        tr("Файлы заявки (*.csr)"));
    if (outFileName != "")
    {
        OpensslWaiting_Genrsa waiting;
        waiting.StartProcess();
        int execStatus = waiting.exec();
        bool isCanceled = waiting.IsCanceledByUser();
        int procStatus = waiting.GetProcessStatus();
        
		if (isCanceled)
			return;

        if (0 == execStatus && 0 == procStatus)
        {

            uint32_t VSNumber = 0;
            if (getSerialNumber(&VSNumber) == 0)
            {
                char CN[128];
                sprintf(CN, "%x", VSNumber);
                // Ключ был сгенерирован успешно, серийник получен
                // Можно приступать к созданию заявки
                OpensslWaiting_Req waiting;
                waiting.StartProcess(
                            outFileName,
                            getCountryName(),
                            stateText,
                            cityText,
                            companyText,
                            departmentText,
                            emailText,
                            CN);
                int execStatus = waiting.exec();
                bool isCanceled = waiting.IsCanceledByUser();
                int procStatus = waiting.GetProcessStatus();
				
				if (isCanceled)
					return;
				
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
                QMessageBox::critical(this, "Ошибка", "Не удалось получить серийный номер системного диска."
                                                      "\nВозможно запуск программы от имени администратора решит эту проблему.");
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

void Registration::showError(const QString &fieldName)
{
    QMessageBox::critical(this, "Ошибка", QString() + "Поле '" + fieldName + "' содержит запрещенные символы. "
        + "\nК вводу запрещены символы национального алфавита и управляющие символы");
}
