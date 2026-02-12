/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: debug.cpp | Created: 2025-04-09
 * Author: sfwidde
 */

#ifdef _DEBUG
#include "debug.hpp"
#include "main.hpp"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

static BOOL   consoleIsOurs;
static HANDLE consoleHandle;

void OpenDebugConsole()
{
	consoleIsOurs = AllocConsole();
	if (consoleIsOurs) // We allocated it
	{
		SetConsoleTitle(TEXT("GTA: Vice City Debug Console (" MOD_BINARY_NAME ")"));

		// Redirect stdin, stdout and stderr to this new console
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void CloseDebugConsole()
{
	if (consoleIsOurs)
	{
		fclose(stderr);
		fclose(stdout);
		fclose(stdin);
		FreeConsole();
		consoleIsOurs = FALSE;
	}
	consoleHandle = nullptr;
}

void OutputDebug(const char* format, ...)
{
	SYSTEMTIME t;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	va_list ap;
	GetLocalTime(&t);
	GetConsoleScreenBufferInfo(consoleHandle, &csbi);

	// Current (local) time
	SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("%02hu:%02hu:%02hu ", t.wHour, t.wMinute, t.wSecond);
	// Name of the mod
	SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	fputs(MOD_BINARY_NAME, stdout);
	// The message itself
	SetConsoleTextAttribute(consoleHandle, csbi.wAttributes);
	fputs(": ", stdout);
	va_start(ap, format);
	vprintf(format, ap);
	putchar('\n');
	va_end(ap);
}
#endif
