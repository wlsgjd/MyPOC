#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

using namespace std;

DWORD GetProcessIdByName(LPCTSTR ProcessName);