#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

using namespace std;

#define PAGE_SIZE 0x1000
//#define REMOVE_ALL_HEADER

BOOL Protect(PVOID BaseAddr, BOOL bActive)
{
	DWORD OldProtect;
	if (!VirtualProtect(BaseAddr, PAGE_SIZE, PAGE_READWRITE, &OldProtect))
		return FALSE;

	PIMAGE_DOS_HEADER dos = PIMAGE_DOS_HEADER(BaseAddr);
	PIMAGE_NT_HEADERS nt;

	if (bActive)
	{
#ifndef REMOVE_ALL_HEADER
		nt = PIMAGE_NT_HEADERS(PBYTE(BaseAddr) + dos->e_lfanew);

		nt->Signature = 0;
		dos->e_lfanew = 0;
		dos->e_magic = 0;
#else
		memset(BaseAddr, 0x00, PAGE_SIZE);
#endif // REMOVE_ALL_HEADER
	}
	else
	{
		// "MZ"
		dos->e_magic = 0x5A4D;

		if (!dos->e_lfanew)
		{
			dos->e_lfanew = sizeof(IMAGE_DOS_HEADER);
		}

		nt = PIMAGE_NT_HEADERS(PBYTE(BaseAddr) + dos->e_lfanew);

		// "PE"
		nt->Signature	= 0x4550;
	}

	VirtualProtect(BaseAddr, PAGE_SIZE, OldProtect, &OldProtect);

	return TRUE;
}

void _tmain()
{
	PVOID BaseAddr = GetModuleHandle(NULL);

	if (Protect(BaseAddr, TRUE))
	{
		_tprintf(_T("Protecting...!!\n"));

		system("pause");

		if (Protect(BaseAddr, FALSE))
		{
			_tprintf(_T("Unprotecting...!!\n"));
		}

		system("pause");
	}
	else
	{
		_tprintf(_T("Failed Protection... 0x%08X\n"), GetLastError());
	}
}
