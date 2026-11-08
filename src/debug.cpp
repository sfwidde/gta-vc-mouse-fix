/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: debug.cpp | Created: 2025-04-09
 * Author: sfwidde (Kelvin)
 */

#ifdef _DEBUG
#include "debug.hpp"
#include "main.hpp"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

static BOOL   g_consoleIsOurs;
static HANDLE g_consoleHandle;

void OpenDebugConsole()
{
	g_consoleIsOurs = AllocConsole();
	if (g_consoleIsOurs) // We allocated it
	{
		SetConsoleTitleW(L"GTA: Vice City Debug Console (" MOD_BINARY_NAME ")");

		// Redirect stdin, stdout and stderr to this new console
		(void)freopen("CONIN$", "r", stdin);
		(void)freopen("CONOUT$", "w", stdout);
		(void)freopen("CONOUT$", "w", stderr);
	}
	g_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void CloseDebugConsole()
{
	if (g_consoleIsOurs)
	{
		fclose(stderr);
		fclose(stdout);
		fclose(stdin);
		FreeConsole();
		g_consoleIsOurs = FALSE;
	}
	g_consoleHandle = nullptr;
}

void OutputDebug(const char* format, ...)
{
	SYSTEMTIME t;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	va_list ap;
	GetLocalTime(&t);
	GetConsoleScreenBufferInfo(g_consoleHandle, &csbi);

	// Current (local) time
	SetConsoleTextAttribute(g_consoleHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("%02hu:%02hu:%02hu ", t.wHour, t.wMinute, t.wSecond);
	// Name of the mod
	SetConsoleTextAttribute(g_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	fputs(MOD_BINARY_NAME, stdout);
	// The message itself
	SetConsoleTextAttribute(g_consoleHandle, csbi.wAttributes);
	fputs(": ", stdout);
	va_start(ap, format);
	vprintf(format, ap);
	putchar('\n');
	va_end(ap);
}
#endif
