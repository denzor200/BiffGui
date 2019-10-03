#ifndef CONTROLINFO_H
#define CONTROLINFO_H

#include <cstdint>
#include <QString>
#include "utils.h"

struct ControlInfo
{
    uint32_t CRC;
    uint32_t Size;
};

inline QString PrintControlInfo(ControlInfo info)
{
    return (Utils::hexStr(reinterpret_cast<uint8_t*>(&info.CRC), sizeof(info.CRC)) + ':' + std::to_string(info.Size)).c_str();
}

#endif // CONTROLINFO_H
