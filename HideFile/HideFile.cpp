#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

using namespace std;

// HideFile.exe [Path]
void _tmain(int argc, const TCHAR* argv[])
{
	if (argc != 2)
	{
		_tprintf(_T("argument error.\n"));
		return;
	}

    SetFileAttributes(argv[1], FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

    printf("success hide.");
    system("pause");
}