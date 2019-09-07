#ifndef GENERATING_H
#define GENERATING_H

#include <QDialog>
#include <QProcess>

namespace Ui {
    class Dialog;
}

class Generating : public QDialog
{
    Q_OBJECT
public:
    explicit Generating(QWidget *parent = nullptr);
    ~Generating();
private slots:
    void slotDataOnStdout();
    void slotFinished(int, QProcess::ExitStatus);
    void on_pushButton_OK_clicked();
    void on_pushButton_Cancel_clicked();

private:
    Ui::Dialog* ui;
    QProcess* m_Process;
};

#endif // GENERATING_H
