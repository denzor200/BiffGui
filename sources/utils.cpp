#include "utils.h"
#include <QTemporaryFile>
#include <sstream>
#include <iomanip>

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

std::string Utils::hexStr(unsigned char *data, int len)
{
  std::stringstream ss;
  ss << std::hex;
  for (int i = 0; i < len; ++i)
    ss << std::setw(2) << std::setfill('0') << (int)data[i];
  return ss.str();
}

Utils::TempFilenameGuard::TempFilenameGuard()
{
    m_TempFilename = Utils::GetNewTempFilename();
}

Utils::TempFilenameGuard::~TempFilenameGuard()
{
    // TODO: потестировать при отсутствии файла, или отсутствии прав
    if (m_TempFilename != "")
        QFile::remove(m_TempFilename);
}
