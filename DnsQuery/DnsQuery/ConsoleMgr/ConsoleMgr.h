#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

typedef enum CONSOLE_TEXT_COLOR : WORD
{
    CONSOLE_TEXT_COLOR_BLACK = 0x00,
    CONSOLE_TEXT_COLOR_DARK_BLUE,
    CONSOLE_TEXT_COLOR_DARK_GREEN,
    CONSOLE_TEXT_COLOR_DARK_SKYBLUE,
    CONSOLE_TEXT_COLOR_DARK_RED,
    CONSOLE_TEXT_COLOR_DARK_VOILET,
    CONSOLE_TEXT_COLOR_DAKR_YELLOW,
    CONSOLE_TEXT_COLOR_GRAY,
    CONSOLE_TEXT_COLOR_DARK_GRAY,
    CONSOLE_TEXT_COLOR_BLUE,
    CONSOLE_TEXT_COLOR_GREEN,
    CONSOLE_TEXT_COLOR_SKYBLUE,
    CONSOLE_TEXT_COLOR_RED,
    CONSOLE_TEXT_COLOR_VIOLET,
    CONSOLE_TEXT_COLOR_YELLOW,
    CONSOLE_TEXT_COLOR_WHITE
}TEXT_COLOR;

const static TCHAR CONSOLE_TITLE_NAME[] = TEXT("Console Application");

class ConsoleMgr
{
private:
    HANDLE hConsole;

	BOOL SetConsoleWindowSize(SIZE_T cols, SIZE_T lines);
	BOOL SetConsoleWindowTitle(const TCHAR* title);
    BOOL SetConsoleWindowTextColor(CONSOLE_TEXT_COLOR color);
	
    BOOL Init(const TCHAR* title);
    void Pause();

public:
	ConsoleMgr();
    ConsoleMgr(const TCHAR* title);
	virtual ~ConsoleMgr();

};

