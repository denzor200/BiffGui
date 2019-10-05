#include "utils.h"
#include <QTemporaryFile>
#include <sstream>
#include <iomanip>
#include "controlinfo.h"

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

extern "C" uint_least32_t Crc32(const unsigned char * buf, size_t len);

bool Utils::TempFilenameGuard::Write(std::function<void (QByteArray *)> writer, ControlInfo *ctrl) const
{
    if (m_TempFilename != "")
    {
        QByteArray memoryFileOut;
        QFile fileOut(m_TempFilename);
        if (fileOut.open(QIODevice::WriteOnly))
        {
            writer(&memoryFileOut);
            fileOut.write(memoryFileOut);

            // crc32
            // TODO: оптимизировать по памяти.
            if (ctrl)
            {
                ctrl->CRC = Crc32(
                   reinterpret_cast<uint8_t*>(const_cast<char*>(memoryFileOut.data())),
                   static_cast<size_t>(memoryFileOut.size()));
                ctrl->Size = static_cast<uint32_t>(memoryFileOut.size());
            }

            fileOut.close();
            return true;
        }
    }
    return false;
}
