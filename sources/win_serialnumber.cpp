
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
#include <windows.h>
int getSerialNumber(DWORD* pVSNumber)
{
    CHAR szBuffer[MAX_PATH] = {0};
    CHAR szVolume[MAX_PATH] = {0};
    DWORD dwVolumeSize = 0;
    if (GetSystemWindowsDirectoryA(szBuffer, MAX_PATH) == 0)
        return -1;
    dwVolumeSize = (DWORD)(strstr(szBuffer, ":\\") + 2 - szBuffer);
    memcpy(szVolume, szBuffer, dwVolumeSize);
    szVolume[dwVolumeSize] = (char)0;
    if (!GetVolumeInformationA(szVolume, NULL, 0,
        pVSNumber, NULL, NULL, NULL, 0))
        return -1;

    return 0;
}
#else
int getSerialNumber(uint32_t* pVSNumber)
{
    if (pVSNumber)
        *pVSNumber = (uint32_t)-1;

    return 0;
}
#endif

