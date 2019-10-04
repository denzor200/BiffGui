#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <QString>

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

        operator QString() const
        {
            return m_TempFilename;
        }
    };
};

#endif // UTILS_H
