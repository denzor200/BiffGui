#ifndef UTILS_H
#define UTILS_H

class QString;

namespace Utils
{
    // Возвращает или путь, или пустую строку
    // Ответственность за удаление временного файла переносится на пользователя
    QString GetNewTempFilename();
};

#endif // UTILS_H
