/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#include "main.h"
#include "utils.h"
#include <windows.h>
#include <string.h>
#include <stdlib.h>

static HWND& gameWnd = *(HWND*)GTA_ADDR_HWND;
static WNDPROC oldWndProc;
static MouseFixSettings mousefixSettings;

static void ReadSettingsFromConfigFile()
{
	// This is the best I could come up with
	bool fieldHasValidValue[3] =
	{
		false, // mouse_x
		false, // mouse_y
		false  // trigger_key
	};

	ConfigFile mousefix_txt;
	if (mousefix_txt.Open(MOUSEFIX_CONFIG_FILE_NAME) == CONFIGFILE_SUCCESS)
	{
		const char* settingName;
		const char* settingValue;
		char* rem;
		while (mousefix_txt.ReadNextLine())
		{
			settingName = mousefix_txt.GetSettingName();
			settingValue = mousefix_txt.GetSettingValue();
			if (!settingName || !settingValue) { continue; }

			if (!stricmp(settingName, "mouse_x"))
			{
				mousefixSettings.mouse_x = strtof(settingValue, &rem);
				fieldHasValidValue[0] = (!*rem) && (mousefixSettings.mouse_x >= 0.0f);
			}
			else if (!stricmp(settingName, "mouse_y"))
			{
				mousefixSettings.mouse_y = strtof(settingValue, &rem);
				fieldHasValidValue[1] = (!*rem) && (mousefixSettings.mouse_y >= 0.0f);
			}
			else if (!stricmp(settingName, "trigger_key"))
			{
				mousefixSettings.trigger_key = (WORD)strtoul(settingValue, &rem, 16);
				fieldHasValidValue[2] =
					(!*rem) &&
					(mousefixSettings.trigger_key >= VK_LBUTTON) &&
					(mousefixSettings.trigger_key <= VK_OEM_CLEAR) &&
					(mousefixSettings.trigger_key != VK_F10);
			}
		}
		mousefix_txt.Close();
	}

	/* Default values in case we failed to read any of the settings */
	if (!fieldHasValidValue[0]) { mousefixSettings.mouse_x     = 0.0f;  }
	if (!fieldHasValidValue[1]) { mousefixSettings.mouse_y     = 1.2f;  }
	if (!fieldHasValidValue[2]) { mousefixSettings.trigger_key = VK_F9; }
}

static void HandleKeyDown(WORD key)
{
	if (key == mousefixSettings.trigger_key)
	{
		// In case user changed any of the settings whilst in-game
		ReadSettingsFromConfigFile();

		// Apply sensitivity values accordingly
		FLOAT newX = (mousefixSettings.mouse_x != 0.0f) ? mousefixSettings.mouse_x : 0.0025f;
		FLOAT newY = (newX * mousefixSettings.mouse_y);
		PatchMem(GTA_ADDR_MOUSE_X, &newX, sizeof(newX));
		PatchMem(GTA_ADDR_MOUSE_Y, &newY, sizeof(newY));
	}
}

static LRESULT CALLBACK MouseFixWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		HandleKeyDown((WORD)wParam);
	default:
		return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
	}
}

static void MouseFixInit()
{
	// Hook into GTA's WndProc
	oldWndProc = (WNDPROC)GetWindowLong(gameWnd, GWL_WNDPROC);
	SetWindowLong(gameWnd, GWL_WNDPROC, (LONG)MouseFixWndProc);

	// Mouse lock fix
	DWORD actualMouseFix = 0xC3C030; // Silent's work
	PatchMem(GTA_ADDR_MOUSE_FUCKING_FIX, &actualMouseFix, sizeof(actualMouseFix));

	// Initial settings setup
	ReadSettingsFromConfigFile();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		MouseFixInit();
	default:
		return TRUE;
	}
}
