#ifndef CONVERTERWAITING_H
#define CONVERTERWAITING_H

#include <QDialog>
#include "srt.h"

namespace Ui {
class ConverterWaiting;
}

#include <QProcess>

class ConverterWaiting : public QDialog
{
    Q_OBJECT

public:
    explicit ConverterWaiting(bool DisableCancel,QWidget *parent = nullptr);
    ~ConverterWaiting();

    virtual int GetProcessStatus() const = 0;
    bool IsCanceledByUser() const {return m_CanceledByUser || GetProcessStatus() == 499 ;}

private slots:
    void ButtonClicked(bool);
    void slotFinished(int, QProcess::ExitStatus);

protected:
    QProcess* getProcess() {return m_Process;}
    void cancel();

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
    int GetProcessStatus() const override {return m_ProcessStatus;}

    bool GetCRC(QString& out) const;
    const QStringList& GetUsersDecisions() const { return m_UsersDecisions; }
    bool GetMarkupType(SrtFormat::MarkupType& mt) const;

private slots:
    void slotDataOnStdout();
    void slotFinished(int, QProcess::ExitStatus);

private:
    bool StdoutReadLines();
    bool HandleCommandFromConverter(int argc, char **argv);

private:
    QStringList m_UsersDecisions; // специально для субтитров .srt

    QString m_CRC;
    bool m_CRC_Initialized = false;

    SrtFormat::MarkupType m_MarkupType;
    bool m_MarkupType_Initialized = false;

    QStringList& m_Persons;

    int m_ProcessStatus = -1;
};

class ConverterWaiting_SaveSubbtitle : public ConverterWaiting
{
    Q_OBJECT
public:
    explicit ConverterWaiting_SaveSubbtitle(QWidget *parent = nullptr);

    void StartProcess(const QString& InFile, const QString &Decisions, const QString& OutFile, const QVector<QPair<QString,QString>>& params);
    int GetProcessStatus() const override {return m_ProcessStatus;}

private slots:
    void slotFinished(int, QProcess::ExitStatus);

private:
    int m_ProcessStatus = -1;
};

class ConverterWaiting_ResetSetting : public ConverterWaiting
{
    Q_OBJECT
public:
    explicit ConverterWaiting_ResetSetting(QWidget *parent = nullptr);

    void StartProcess();
    int GetProcessStatus() const override {return m_ProcessStatus;}

private slots:
    void slotFinished(int, QProcess::ExitStatus);

private:
    int m_ProcessStatus = -1;
};



#endif // CONVERTERWAITING_H
