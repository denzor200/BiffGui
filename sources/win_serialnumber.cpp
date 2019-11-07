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
