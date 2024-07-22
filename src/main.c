/*
 * Mouse sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([R3V]Kelvin)
 * 2024-01-10
 */

#include "main.h"
#include "utils.h"
#include <Windows.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#ifdef _DEBUG
#include <stdio.h>
#endif

static WNDPROC oldWndProc;
static SensAdjSettings adjusterSettings;

static void ReadAdjusterSettingsFromConfigFile(void)
{
	// This is the best I could come up with.
	bool hasFieldValidValue[3] =
	{
		false, // mouse_x
		false, // mouse_y
		false  // trigger_key
	};

	ConfigFile* mouseSensAdjTxt;
	if (OpenConfigFile(&mouseSensAdjTxt, SENSADJ_CONFIG_FILE_NAME) == CONFIGFILE_SUCCESS)
	{
		const char* settingName;
		const char* settingValue;
		char* rem;
		while (ReadNextConfigFileLine(mouseSensAdjTxt))
		{
			settingName = GetConfigFileSettingName(mouseSensAdjTxt);
			settingValue = GetConfigFileSettingValue(mouseSensAdjTxt);
			if (!settingName || !settingValue) { continue; }

			if (!stricmp(settingName, "mouse_x"))
			{
				adjusterSettings.mouse_x = strtof(settingValue, &rem);
				hasFieldValidValue[0] = (!*rem) && (adjusterSettings.mouse_x >= 0.0f);
			}
			else if (!stricmp(settingName, "mouse_y"))
			{
				adjusterSettings.mouse_y = strtof(settingValue, &rem);
				hasFieldValidValue[1] = (!*rem) && (adjusterSettings.mouse_y >= 0.0f);
			}
			else if (!stricmp(settingName, "trigger_key"))
			{
				adjusterSettings.trigger_key = (int)strtol(settingValue, &rem, 16);
				hasFieldValidValue[2] =
					(!*rem) &&
					(adjusterSettings.trigger_key >= VK_LBUTTON) &&
					(adjusterSettings.trigger_key <= VK_OEM_CLEAR) &&
					(adjusterSettings.trigger_key != VK_F10);
			}
		}
		CloseConfigFile(mouseSensAdjTxt);
	}

	/* Default values in case we failed to read any of the settings */
	if (!hasFieldValidValue[0]) { adjusterSettings.mouse_x     = 0.0f;             }
	if (!hasFieldValidValue[1]) { adjusterSettings.mouse_y     = 1.2f;             }
	if (!hasFieldValidValue[2]) { adjusterSettings.trigger_key = 0x78 /* VK_F9 */; }
#ifdef _DEBUG
	CHAR t[64];
	snprintf(t, sizeof(t),
		"mouse_x: %f\n"
		"mouse_y: %f\n"
		"trigger_key: 0x%02X",
		adjusterSettings.mouse_x,
		adjusterSettings.mouse_y,
		(unsigned int)adjusterSettings.trigger_key);
	MessageBoxA(GTA_HWND, t, "Hi", MB_OK);
#endif
}

static void HandleKeyDown(int key)
{
	if (key == adjusterSettings.trigger_key)
	{
		// In case user changed any of the settings whilst in-game.
		ReadAdjusterSettingsFromConfigFile();

		// Apply sensitivity settings accordingly.
		FLOAT newX = (adjusterSettings.mouse_x != 0.0f) ? adjusterSettings.mouse_x : 0.0025f;
		FLOAT newY = (newX * adjusterSettings.mouse_y);
		ModifyGTAMemory(&GTA_MOUSE_X, &newX, sizeof(GTA_MOUSE_X));
		ModifyGTAMemory(&GTA_MOUSE_Y, &newY, sizeof(GTA_MOUSE_Y));
	}
}

static LRESULT CALLBACK NewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		HandleKeyDown((int)wParam);
	default:
		return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		ReadAdjusterSettingsFromConfigFile(); // Initial setup.

		oldWndProc = (WNDPROC)GetWindowLong(GTA_HWND, GWL_WNDPROC);
		SetWindowLong(GTA_HWND, GWL_WNDPROC, (LONG)NewWndProc);
	default:
		return TRUE;
	}
}
