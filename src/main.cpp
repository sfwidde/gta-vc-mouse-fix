/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#include "gamepatch.hpp"
#include "util/io.hpp"
#include "util/common.hpp"
#include <windows.h>
#include <string.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------

static WNDPROC oldWndProc;
static GameMem<FLOAT>* gameMouseX; // x-axis
static GameMem<FLOAT>* gameMouseY; // y-axis
static struct
{
	float mouse_x;    // VC:MP 0.4 equivalent: game_sensitivity
	float mouse_y;    // VC:MP 0.4 equivalent: game_sensitivity_ratio
	WORD  reload_key; // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
} mouseFixSettings;

// -----------------------------------------------------------------------------

static void ReadMouseFixSettingsFromConfigFile()
{
	// This is the best I could come up with
	bool fieldHasValidValue[3] =
	{
		false, // mouse_x
		false, // mouse_y
		false  // reload_key
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
			else if (!stricmp(settingName, "reload_key"))
			{
				mouseFixSettings.reload_key = (WORD)strtoul(settingValue, &rem, 16);
				fieldHasValidValue[2] =
					(!*rem) &&
					(mouseFixSettings.reload_key >= VK_LBUTTON) &&
					(mouseFixSettings.reload_key <= VK_OEM_CLEAR) &&
					(mouseFixSettings.reload_key != VK_F10);
			}
		}
		mousefix_txt.Close();
	}

	/* Default values in case we failed to read any of the settings */
	if (!fieldHasValidValue[0]) { mouseFixSettings.mouse_x    = 0.0f;  }
	if (!fieldHasValidValue[1]) { mouseFixSettings.mouse_y    = 1.2f;  }
	if (!fieldHasValidValue[2]) { mouseFixSettings.reload_key = VK_F9; }
}

// -----------------------------------------------------------------------------

static void OnGameFrame()
{
	// Don't question me, this is the awfully awful way VC:MP 0.4 has been doing it
	if (mouseFixSettings.mouse_x != 0.0f) { gameMouseX->value = mouseFixSettings.mouse_x; }
	gameMouseY->value = (gameMouseX->value * mouseFixSettings.mouse_y);
}

static void OnKeyDown(WORD key)
{
	// Reload settings now
	if (key == mouseFixSettings.reload_key) { ReadMouseFixSettingsFromConfigFile(); }
}

// -----------------------------------------------------------------------------
// HOOK PROCEDURES
// -----------------------------------------------------------------------------

static LRESULT CALLBACK MouseFixWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		OnKeyDown((WORD)wParam);
	default:
		return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
	}
}

static NAKED CGame_ProcessHook()
{
	__asm
	{
		; The actual hook call
		pushad
		call OnGameFrame
		popad

		; First 7 bytes of CGame::Process() (0x4A4410)
		push ebx          ; 53
		push ebp          ; 55
		mov eax, 0x4AB6C0
		call eax          ; E8 A9 72 00 00

		; Restore execution at 0x4A4410 + 7
		mov eax, 0x4A4417
		jmp eax
	}
}

// -----------------------------------------------------------------------------

static void MouseFixInit()
{
	/*
		Credits to whoever originally found out about
		the addresses we are about to use below
	 */

	// Create a(nother - other DLLs could have done this themselves too) WndProc subclass
	oldWndProc = (WNDPROC)SetWindowLong(*(HWND*)0x7897A4, GWL_WNDPROC, (LONG)MouseFixWndProc);
	
	// Initial settings setup
	ReadMouseFixSettingsFromConfigFile();

	// Mouse lock fix taken from Silent's SilentPatch:
	// https://github.com/CookiePLMonster/SilentPatch/blob/2a597da1bc2b974082c8b1fc13c08788b42615af/SilentPatchVC/SilentPatchVC.cpp#L2134
	GameMem<DWORD>(0x601740).value = 0xC3C030;

	// Thx Hanney for the addresses
	gameMouseX = new GameMem<FLOAT>(0x94DBD0);
	gameMouseY = new GameMem<FLOAT>(0xA0D964);

	// Thx AdTec_224 for the address
	InstallGameFunctionHook<2>(0x4A4410, CGame_ProcessHook);
}

static void MouseFixFinal()
{
	// DllMain has some limitations, and my logic tells me not to call any Win32 API function
	// before DLL_PROCESS_ATTACH and/or after DLL_PROCESS_DETACH; therefore, we do this not to call
	// VirtualProtect() indirectly through GameMem's constructor/destructor in the case these variables
	// were defined as global, which is what I would have done in the first place
	delete gameMouseY;
	delete gameMouseX;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		MouseFixInit();
		break;

	case DLL_PROCESS_DETACH:
		MouseFixFinal();
		break;
	}
	return TRUE;
}

// -----------------------------------------------------------------------------
