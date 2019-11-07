#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include <QObject>

namespace Ui {
    class Registration;
}

class Registration : public QDialog
{
    Q_OBJECT
public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();
    QString getCountryName() const;
    static bool validateString(const QString& value);
private slots:
    void on_pushButton_Continue_clicked();
    void on_pushButton_2_clicked();
private:
    void showError(const QString& fieldName);

private:
    Ui::Registration* ui;
};

#endif // REGISTRATION_H
