#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntimage.h>
#include <minwindef.h>
#pragma comment(lib, "ntdll.lib")

BOOL FindProcessByName(CHAR* process_name, HANDLE* pid);