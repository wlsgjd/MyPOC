/*********************************************************
				Name:			FileManager
				Author:			Jinhyeong,Choe (cshelldll)
				Date:			2024.04.23
				Description:	File I/O Manager.
**********************************************************/

#pragma once
#include <Windows.h>
#include <tchar.h>

class FileManager
{
private:
	HANDLE hFile;
	BYTE*	buffer;
	
public:
	SIZE_T	size;

public:
	FileManager();
	virtual ~FileManager();

	BOOL Open(const TCHAR* path);
	BOOL Close();

	BOOL Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LONG lDistanceToMove, LONG dwMoveMethod);

	BYTE* operator[](int i) { return &buffer[i]; }

};