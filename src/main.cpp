/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

/*
	Many thanks =) to
	- Hanney (Providing mouse X and Y addresses)
	- Silent (Mouse lock fix based off his code)
	- AdTec_224 (Providing CGame::Process() address)
 */

#include "gamepatch.hpp"
#include "util/io.hpp"
#include "util/common.hpp"
#include <windows.h>
#include <string.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------

static WNDPROC oldWndProc; // Original WndProc procedure before subclassing
static GameMem<FLOAT, false> gameMouseX(0x94DBD0); // x-axis mouse sensitivity
static GameMem<FLOAT, false> gameMouseY(0xA0D964); // y-axis mouse sensitivity
static GameHook<7> gameLoopHook(0x4A4410); // CGame::Process() hook
static struct // User settings loaded into memory
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
	// Don't question me, this is the (awfully awful) way VC:MP 0.4 has been doing it
	if (mouseFixSettings.mouse_x != 0.0f) { gameMouseX.value = mouseFixSettings.mouse_x; }
	gameMouseY.value = (gameMouseX.value * mouseFixSettings.mouse_y);
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
		[[fallthrough]];
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
	// Create a(nother - other DLLs could have done this themselves too) WndProc subclass
	oldWndProc = (WNDPROC)SetWindowLong(GameMem<HWND>(0x7897A4).value, GWL_WNDPROC, (LONG)MouseFixWndProc);
	
	// Initial settings setup
	ReadMouseFixSettingsFromConfigFile();

	// Mouse lock fix taken from Silent's SilentPatch:
	// https://github.com/CookiePLMonster/SilentPatch/blob/2a597da1bc2b974082c8b1fc13c08788b42615af/SilentPatchVC/SilentPatchVC.cpp#L2134
	GameMem<DWORD>(0x601740).value = 0xC3C030;

	// We avoid calling VirtualProtect() before DllMain this way
	gameMouseX.Unprotect();
	gameMouseY.Unprotect();

	// Install CGame::Process() hook
	gameLoopHook.Install(CGame_ProcessHook);
}

static void MouseFixFinal()
{
	// Revert whatever changes we made
	gameLoopHook.Uninstall({0x53, 0x55, 0xE8, 0xA9, 0x72, 0x00, 0x00});
	gameMouseY.Reprotect();
	gameMouseX.Reprotect();
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
