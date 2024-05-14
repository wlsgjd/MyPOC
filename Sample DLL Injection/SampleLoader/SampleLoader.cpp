/*********************************************************
				Name:			SampleLoader
				Author:			Jinhyeong,Choe (cshelldll)
				Date:			2024.04.17
				Description:	Sample DLL Injector
**********************************************************/

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include "InjectionMgr/InjectionMgr.h"

#pragma comment(lib, "shlwapi")

inline BOOL GetInjectionInfo(int argc, const TCHAR* argv[], InjectionInfo& info)
{
	if (argc == 3)
	{
		if (_tcscmp(argv[1], _T("-Catalog5")) == 0)
		{
			info.type = INJECTION_TYPE_WINSOCK2_CATALOG5;
		}
		else if (_tcscmp(argv[1], _T("-Catalog9")) == 0)
		{
			info.type = INJECTION_TYPE_WINSOCK2_CATALOG9;
		}
		else
		{
			return FALSE;
		}

		_tcscpy_s(info.path, sizeof(info.path), argv[2]);
	}
	else if (argc == 2)
	{
		if (_tcscmp(argv[1], _T("-Reset")) == 0)
		{
			info.type = INJECTION_TYPE_WINSOCK2_RESET;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

inline BOOL CopyInjectionFile(InjectionInfo& info)
{
	TCHAR path[MAX_PATH];
	GetWindowsDirectory(path, sizeof(path));
	PathAppend(path, PathFindFileName(info.path));

	if (!CopyFile(info.path, path, FALSE))
	{
		return FALSE;
	}

	_tcscpy_s(info.path, sizeof(info.path), path);
	return TRUE;
}

void _tmain(int argc, const TCHAR* argv[])
{
	InjectionInfo info;
	if (!GetInjectionInfo(argc, argv, info))
	{
		_tprintf(_T(
			"Argument Error.\n\n"
			"Example)\n"
			"1. SampleLoader.exe -Catalog5 [DLL Path]\n"
			"2. SampleLoader.exe -Catalog9 [DLL Path]\n"
			"3. SampleLoader.exe -Reset\n"
		));
		return;
	}

	InjectionMgr injectionMgr;
	if (INJECTION_TYPE_WINSOCK2_RESET == info.type)
	{
		injectionMgr.Reset();
	}
	else
	{
		if (!CopyInjectionFile(info))
		{
			_tprintf(_T("Copy failed.\n"));
			return;
		}

		if (!injectionMgr.InjectDLL(info.type, info.path))
		{
			_tprintf(_T("Injection failed.\n"));

			DeleteFile(info.path);
			return;
		}
	}

	_tprintf(_T("complete!\n"));
}