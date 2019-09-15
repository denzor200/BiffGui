#include "utils.h"
#include <QTemporaryFile>

QString Utils::GetNewTempFilename()
{
    QTemporaryFile Subtitle;
    if (Subtitle.open())
    {
        Subtitle.setAutoRemove(false);
        return Subtitle.fileName();
    }
    return "";
}
