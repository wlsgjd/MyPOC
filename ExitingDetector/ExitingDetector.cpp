#include "ExitingDetector.h"

int main()
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, GetProcessIdByName(_T("notepad.exe")));
    NtQueryInformationProcessT NtQueryInformationProcess = (NtQueryInformationProcessT)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtQueryInformationProcess");

    PROCESS_EXTENDED_BASIC_INFORMATION info = { 0x00, };
    ULONG len;
    NtQueryInformationProcess(hProcess, ProcessBasicInformation, &info, sizeof(info), &len);

    if (info.IsProcessDeleting)
    {
        printf("ProcessSelfDelete, ProcessDelete : True\n");

        DWORD exitcode = 0;
        GetExitCodeProcess(hProcess, &exitcode);
        if (exitcode == STILL_ACTIVE)
        {
            printf("detected !!\n");
        }
    }

    ULONG mode = 0;
    NtQueryInformationProcess(hProcess, ProcessMemoryAllocationMode, &mode, sizeof(mode), &len);
    if (mode)
    {
        printf("VmTopMode : True\n");
    }

    printf("success. \n");
    system("pause");
}
