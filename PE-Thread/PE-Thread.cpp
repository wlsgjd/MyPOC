#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

using namespace std;

#define PAGE_SIZE 0x1000
//#define REMOVE_ALL_HEADER

BOOL ProtectThreadCreation(PVOID BaseAddr)
{
	DWORD OldProtect;
	if (!VirtualProtect(BaseAddr, PAGE_SIZE, PAGE_READWRITE, &OldProtect))
		return FALSE;

#ifndef REMOVE_ALL_HEADER
	PIMAGE_DOS_HEADER dos	= PIMAGE_DOS_HEADER(BaseAddr);
	PIMAGE_NT_HEADERS nt	= PIMAGE_NT_HEADERS(PBYTE(BaseAddr) + dos->e_lfanew);

	nt->Signature	= 0;
	dos->e_lfanew	= 0;
	dos->e_magic	= 0;
#else
	memset(BaseAddr, 0x00, PAGE_SIZE);
#endif // REMOVE_ALL_HEADER

	return VirtualProtect(BaseAddr, PAGE_SIZE, OldProtect, &OldProtect);
}

void _tmain()
{
	PVOID BaseAddr = GetModuleHandle(NULL);

	if (ProtectThreadCreation(BaseAddr))
	{
		_tprintf(_T("Protecting...!!\n"));

		system("pause");
	}
	else
	{
		_tprintf(_T("Failed Protection... 0x%08X\n"), GetLastError());
	}
}
