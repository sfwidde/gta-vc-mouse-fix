/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#include "util/io.hpp"
#include "gamepatch.hpp"
#include <windows.h>
#include <string.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------

static WNDPROC oldWndProc;
static struct
{
	float mouse_x;     // VC:MP 0.4 equivalent: game_sensitivity
	float mouse_y;     // VC:MP 0.4 equivalent: game_sensitivity_ratio
	WORD  trigger_key; // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
} mouseFixSettings;

// -----------------------------------------------------------------------------

static void ReadMouseFixSettingsFromConfigFile()
{
	// This is the best I could come up with
	bool fieldHasValidValue[3] =
	{
		false, // mouse_x
		false, // mouse_y
		false  // trigger_key
	};

	ConfigFile mousefix_txt;
	if (mousefix_txt.Open("mousefix.txt") == CONFIGFILE_SUCCESS)
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
				mouseFixSettings.mouse_x = strtof(settingValue, &rem);
				fieldHasValidValue[0] = (!*rem) && (mouseFixSettings.mouse_x >= 0.0f);
			}
			else if (!stricmp(settingName, "mouse_y"))
			{
				mouseFixSettings.mouse_y = strtof(settingValue, &rem);
				fieldHasValidValue[1] = (!*rem) && (mouseFixSettings.mouse_y >= 0.0f);
			}
			else if (!stricmp(settingName, "trigger_key"))
			{
				mouseFixSettings.trigger_key = (WORD)strtoul(settingValue, &rem, 16);
				fieldHasValidValue[2] =
					(!*rem) &&
					(mouseFixSettings.trigger_key >= VK_LBUTTON) &&
					(mouseFixSettings.trigger_key <= VK_OEM_CLEAR) &&
					(mouseFixSettings.trigger_key != VK_F10);
			}
		}
		mousefix_txt.Close();
	}

	/* Default values in case we failed to read any of the settings */
	if (!fieldHasValidValue[0]) { mouseFixSettings.mouse_x     = 0.0f;  }
	if (!fieldHasValidValue[1]) { mouseFixSettings.mouse_y     = 1.2f;  }
	if (!fieldHasValidValue[2]) { mouseFixSettings.trigger_key = VK_F9; }
}

// -----------------------------------------------------------------------------

static void HandleKeyDown(WORD key)
{
	if (key == mouseFixSettings.trigger_key)
	{
		// In case user changed any of the settings whilst in-game
		ReadMouseFixSettingsFromConfigFile();

		// Thanks Hanney for providing these addresses and of course
		// credits to whoever found out about them in the first place
		GameMem<FLOAT> gameMouseX(0x94DBD0);
		GameMem<FLOAT> gameMouseY(0xA0D964);
		// Apply sensitivity values accordingly
		gameMouseX.value = ((mouseFixSettings.mouse_x != 0.0f) ? mouseFixSettings.mouse_x : 0.0025f);
		gameMouseY.value = (gameMouseX.value * mouseFixSettings.mouse_y);
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

// -----------------------------------------------------------------------------

static BOOL MouseFixInit()
{
	// Attempt to create a(nother - other DLLs could have done this themselves too) WndProc subclass
	oldWndProc = (WNDPROC)SetWindowLong(*(HWND*)0x7897A4, GWL_WNDPROC, (LONG)MouseFixWndProc);
	// We don't want to take up user's RAM if we are of no use
	if (!oldWndProc) { return FALSE; }

	// Initial settings setup
	ReadMouseFixSettingsFromConfigFile();

	// Mouse lock fix taken from Silent's SilentPatch:
	// https://github.com/CookiePLMonster/SilentPatch/blob/2a597da1bc2b974082c8b1fc13c08788b42615af/SilentPatchVC/SilentPatchVC.cpp#L2134
	// -- could be ThirteenAG's (or somebody else's) work though, credits to whoever deserves them in this case
	GameMem<DWORD>(0x601740).value = 0xC3C030;

	// TODO: Hook the game's main loop to apply sensitivity values when a game is loaded
	// and make it compatible with both singleplayer and 0.3z R2

	// Success
	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		return MouseFixInit();

	default:
		return TRUE;
	}
}

// -----------------------------------------------------------------------------
