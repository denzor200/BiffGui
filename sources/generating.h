#ifndef GENERATING_H
#define GENERATING_H

#include <QDialog>
#include <QProcess>

namespace Ui {
    class Generating;
}

class Generating : public QDialog
{
    Q_OBJECT
public:
    explicit Generating(QWidget *parent = nullptr);
    ~Generating();

    void StartProcess(const QString& InFile, const QString& ConfigFile, const QString& OutDir);

private slots:
    void slotDataOnStdout();
    void slotFinished(int, QProcess::ExitStatus);
    void on_pushButton_OK_clicked();
    void on_pushButton_Cancel_clicked();

private:
    Ui::Generating* ui;
    QProcess* m_Process;
};

#endif // GENERATING_H
