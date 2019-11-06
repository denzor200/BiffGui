#ifndef AUTH_H
#define AUTH_H

#include <QDialog>

namespace Ui {
class Auth;
}

class Auth : public QDialog
{
    Q_OBJECT

public:
    explicit Auth(QWidget *parent = nullptr);
    ~Auth();

private slots:
    void on_pushButton_makeRequest_clicked();

    void on_commandLinkButton_Continue_clicked();

    void on_toolButton_CertFile_clicked();

private:
    Ui::Auth *ui;
};

#endif // AUTH_H
