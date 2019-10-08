#include "utils.h"
#include <QTemporaryFile>
#include <sstream>
#include <iomanip>
#include "controlinfo.h"
#include "build_defs.h"

static const char completeVersion[] =
{
#if defined(VERSION_MAJOR_INIT) && defined(VERSION_MINOR_INIT)
    VERSION_MAJOR_INIT,
    '.',
    VERSION_MINOR_INIT,
    '-', 'V', '-',
#endif
    BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
    '-',
    BUILD_MONTH_CH0, BUILD_MONTH_CH1,
    '-',
    BUILD_DAY_CH0, BUILD_DAY_CH1,
    ' ',
    BUILD_HOUR_CH0, BUILD_HOUR_CH1,
    ':',
    BUILD_MIN_CH0, BUILD_MIN_CH1,
    ':',
    BUILD_SEC_CH0, BUILD_SEC_CH1,
    '\0'
};

const char* Utils::compileDatetime()
{
    return completeVersion;
}

std::string Utils::compilerString()
{
#if defined(__clang__) // must be before GNU, because clang claims to be GNU too
    std::string isAppleString;
#if defined(__apple_build_version__) // Apple clang has other version numbers
    isAppleString = (" (Apple)");
#endif
    return std::string("Clang ") + std::to_string(__clang_major__) + ('.')
        + std::to_string(__clang_minor__) + isAppleString;
#elif defined(__GNUC__) || defined(__MINGW32__)
    return std::string("GCC ") + (__VERSION__);
#elif defined(_MSC_VER)
    if (_MSC_VER > 1999)
        return std::string("MSVC <unknown>");
    if (_MSC_VER >= 1910)
        return std::string("MSVC 2017");
    if (_MSC_VER >= 1900)
        return std::string("MSVC 2015");
    if (_MSC_VER >= 1800) // 1800: MSVC 2013 (yearly release cycle)
        return std::string("MSVC ") + std::to_string(2008 + ((_MSC_VER / 100) - 13));
    if (_MSC_VER >= 1500) // 1500: MSVC 2008, 1600: MSVC 2010, ... (2-year release cycle)
        return std::string("MSVC ") + std::to_string(2008 + 2 * ((_MSC_VER / 100) - 15));
#endif
    return std::string("<unknown compiler>");
}

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

int Utils::stoi(
        const std::string& value,int def,size_t* idx,int base)
{
    try {
        return std::stoi(value, idx, base);
    }
    catch (const std::invalid_argument&)
    {
    }
    catch (const std::out_of_range&)
    {
    }
    return def;
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
