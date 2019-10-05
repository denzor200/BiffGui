#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <QString>
#include <functional>

struct ControlInfo;

namespace Utils
{
    // Возвращает или путь, или пустую строку
    // Ответственность за удаление временного файла переносится на пользователя
    QString GetNewTempFilename();

    std::string hexStr(unsigned char *data, int len);

    class TempFilenameGuard
    {
        QString m_TempFilename;
    public:
        TempFilenameGuard();
        ~TempFilenameGuard();

        bool Write(std::function<void(QByteArray*)> writer,ControlInfo* ctrl) const;

        operator QString() const { return m_TempFilename; }
    };
};

#endif // UTILS_H
