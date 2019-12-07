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

    void Log(const QString& line);
    void LogStage(const QString& line);

    void SetMainStatus(const QString& line);

    void StartProcess(const QString& InFile, const QString& Decisions, const QString& TableFile, const QString& OutDir, const QVector<QPair<QString,QString>>& params);

private:
    void StdoutReadLines();
    void StderrReadLines();
    void HandleCommandFromConverter(int argc, char** argv);

private slots:
    void slotDataOnStdout();
    void slotDataOnStderr();
    void slotFinished(int, QProcess::ExitStatus);
    void on_pushButton_OK_clicked();
    void on_pushButton_Cancel_clicked();

private:
    Ui::Generating* ui;
    QProcess* m_Process;

    QHash<QString, int> m_DomentIds;
    unsigned long m_CurDocumentCount = 0;
    unsigned long m_MaxDocumentsCount = 0;
};

#endif // GENERATING_H
