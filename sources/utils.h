#ifndef UTILS_H
#define UTILS_H

class QString;
#include <string>

namespace Utils
{
    // Возвращает или путь, или пустую строку
    // Ответственность за удаление временного файла переносится на пользователя
    QString GetNewTempFilename();

    std::string hexStr(unsigned char *data, int len);
};

#endif // UTILS_H
