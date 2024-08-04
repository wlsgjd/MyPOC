/*********************************************************
				Name:			CopyCert
				Author:			Jinhyeong,Choe (wlsgjd)
				Date:			2024.04.23
				Description:	PE 인증서 복사 도구.
**********************************************************/

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <ImageHlp.h>
#include "./FileManager/FileManager.h"

using namespace std;

void _tmain(int argc, const TCHAR* argv[])
{
	PIMAGE_NT_HEADERS		pNtHeaders;
	PIMAGE_DATA_DIRECTORY	pSecurityDir;
	DWORD dwSecDirOffset, dwSecDirSize;

	if (argc < 3)
	{
		_tprintf(_T(
			"Argument Error.\n\n"
			"Example)\n"
			"1. CopyCert.exe [src_file] [dest_file]\n"
		));
		return;
	}

	FileManager SrcFile, DestFile;
	if (!(SrcFile.Open(argv[1]) & DestFile.Open(argv[2])))
	{
		_tprintf(_T("Open Error\n"), GetLastError());
		return;
	}

	// Source File
	pNtHeaders = PIMAGE_NT_HEADERS(SrcFile[0] + PIMAGE_DOS_HEADER(SrcFile[0])->e_lfanew);
	pSecurityDir = &pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];

	dwSecDirOffset = pSecurityDir->VirtualAddress;
	dwSecDirSize = pSecurityDir->Size;

	// Destination File
	pNtHeaders = PIMAGE_NT_HEADERS(DestFile[0] + PIMAGE_DOS_HEADER(DestFile[0])->e_lfanew);
	pSecurityDir = &pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
	pSecurityDir->VirtualAddress = DestFile.size;
	pSecurityDir->Size = dwSecDirSize;

	DestFile.Write(pNtHeaders, sizeof(IMAGE_NT_HEADERS64), PIMAGE_DOS_HEADER(DestFile[0])->e_lfanew, FILE_BEGIN);
	DestFile.Write(SrcFile[dwSecDirOffset], dwSecDirSize, 0, FILE_END);

	_tprintf(_T("copy success.\n"));
}