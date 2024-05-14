/*********************************************************
                Name:			Sample DLL
                Author:			Jinhyeong,Choe (cshelldll)
                Date:			2024.04.17
                Description:	Anti-Cheat Bypass Sample.
**********************************************************/

#include <Windows.h>
#include <stdio.h>
#include <tchar.h> 
#include <shlwapi.h>
#include "InjectionMgr/InjectionMgr.h"

#pragma comment(lib, "shlwapi")

InjectionMgr injectionMgr;

BOOL CheckProcessName(const TCHAR* name)
{
    TCHAR process_name[MAX_PATH];

    if (!GetModuleFileName(NULL, process_name, sizeof(process_name)))
    {
        return FALSE;
    }

    if (_tcscmp(PathFindFileName(process_name), name) != 0)
    {
        return FALSE;
    }

    return TRUE;
}

// 제거 시 관리자 권한 필요.
void RemoveInjectionInfo()
{
    injectionMgr.Reset();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            if (CheckProcessName(_T("TslGame.exe")))
            {
                MessageBoxA(NULL, "Injected.", "SampleDLL", MB_OK);
                RemoveInjectionInfo();
            }
        }break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}