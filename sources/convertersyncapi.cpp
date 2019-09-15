#include "convertersyncapi.h"
#include <QProcess>

int ConverterSyncAPI::ShowPersonList(const QString &SubbtitlePath, const QString &OutPath)
{
    QProcess process;
    // TODO: make normal command
    // TODO: потестить на открытии файла, который не является субтитрой
    QStringList Arguments;
    Arguments.reserve(3);
    Arguments.push_back("-show_person_list");
    Arguments.push_back(SubbtitlePath);
    Arguments.push_back(OutPath);
    return QProcess::execute("D:\\repos\\subtitles\\Debug\\converter", Arguments);
}
