#include "convertersyncapi.h"
#include <QProcess>
#include <QDebug>

// не используется..

int ConverterSyncAPI::ShowPersonList(QStringList& PersonsList,const QString &SubbtitlePath)
{
    QProcess process;
    // TODO: make normal command
    // TODO: потестить на открытии файла, который не является субтитрой
    QStringList Arguments;
    Arguments.reserve(2);
    Arguments.push_back("-show_person_list");
    Arguments.push_back(SubbtitlePath);
    process.start("D:\\repos\\subtitles\\Debug\\converter", Arguments);
    process.setReadChannel(QProcess::StandardOutput);
    process.waitForReadyRead();
    while (process.canReadLine())
    {
        PersonsList << QString::fromUtf8(process.readLine());
    }
    process.waitForFinished(-1);
    return process.exitCode();
}
