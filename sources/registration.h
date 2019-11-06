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
private slots:
    void on_pushButton_Continue_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::Registration* ui;
};

#endif // REGISTRATION_H
