#include "ConsoleMgr.h"

BOOL ConsoleMgr::SetConsoleWindowSize(SIZE_T cols, SIZE_T lines)
{
	TCHAR command[64];
	_stprintf_s(command, _countof(command), TEXT("mode con: cols=%d lines=%d"), cols, lines);

	return _tsystem(command);
}

BOOL ConsoleMgr::SetConsoleWindowTitle(const TCHAR* title)
{
	return SetConsoleTitle(title);
}

BOOL ConsoleMgr::SetConsoleWindowTextColor(CONSOLE_TEXT_COLOR color)
{
	return SetConsoleTextAttribute(hConsole, color);
}

ConsoleMgr::ConsoleMgr()
{
	Init(CONSOLE_TITLE_NAME);
}

ConsoleMgr::ConsoleMgr(const TCHAR* title)
{
	Init(title);
}

ConsoleMgr::~ConsoleMgr()
{
	Pause();
	CloseHandle(hConsole);
}

BOOL ConsoleMgr::Init(const TCHAR* title)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleWindowTitle(title);
	SetConsoleWindowTextColor(CONSOLE_TEXT_COLOR_DARK_GREEN);

	return TRUE;
}

void ConsoleMgr::Pause()
{
	_tsystem(TEXT("pause"));
}
