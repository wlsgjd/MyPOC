#include "FileManager.h"

FileManager::FileManager() : hFile(INVALID_HANDLE_VALUE), buffer(NULL)
{
}

FileManager::~FileManager()
{
	this->Close();
}

BOOL FileManager::Open(const TCHAR* path)
{
	hFile = CreateFile(path, FILE_GENERIC_READ | FILE_GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// Source File
	size = GetFileSize(hFile, NULL);
	buffer = (BYTE*)malloc(size);

	DWORD dwReadWriteBytes;
	ReadFile(hFile, buffer, size, &dwReadWriteBytes, NULL);

	return TRUE;
}

BOOL FileManager::Close()
{
	if (buffer)
	{
		free(buffer);
	}

	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	return TRUE;
}

BOOL FileManager::Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LONG lDistanceToMove, LONG dwMoveMethod)
{
	SetFilePointer(hFile, lDistanceToMove, NULL, dwMoveMethod);

	DWORD dwReadWriteBytes;
	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &dwReadWriteBytes, NULL);
}
