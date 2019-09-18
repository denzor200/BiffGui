#ifndef CONVERTERWAITING_H
#define CONVERTERWAITING_H

#include <QDialog>

namespace Ui {
class ConverterWaiting;
}

#include <QProcess>

class ConverterWaiting : public QDialog
{
    Q_OBJECT

public:
    explicit ConverterWaiting(QWidget *parent = nullptr);
    ~ConverterWaiting();

    bool IsCanceledByUser() const {return m_CanceledByUser;}

private slots:
    void ButtonClicked(bool);
    void slotFinished(int, QProcess::ExitStatus);

protected:
    QProcess* getProcess() {return m_Process;}

private:
    Ui::ConverterWaiting *ui;
    bool        m_CanceledByUser = false;
    QProcess*   m_Process = nullptr;
};

class ConverterWaiting_ShowPersonList : public ConverterWaiting
{
    Q_OBJECT
public:
    explicit ConverterWaiting_ShowPersonList(QStringList& Persons,QWidget *parent = nullptr);

    void StartProcess( const QString &SubbtitlePath);
    int GetProcessStatus() const {return m_ProcessStatus;}

private slots:
    void slotDataOnStdout();
    void slotFinished(int, QProcess::ExitStatus);

private:
    QStringList& m_Persons;
    int m_ProcessStatus = -1;
};



#endif // CONVERTERWAITING_H
