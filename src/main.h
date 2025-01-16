/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-05-15
 */

#pragma once

#include <windows.h>

// -----------------------------------------------------------------------------

#define GTA_ADDR_HWND              0x7897A4 // HWND
// Thx Silent (https://github.com/CookiePLMonster/SilentPatch/blob/2a597da1bc2b974082c8b1fc13c08788b42615af/SilentPatchVC/SilentPatchVC.cpp#L2134)
#define GTA_ADDR_MOUSE_FUCKING_FIX 0x601740 // DWORD
// Thx Hanney for these two below
#define GTA_ADDR_MOUSE_X           0x94DBD0 // FLOAT
#define GTA_ADDR_MOUSE_Y           0xA0D964 // FLOAT

// -----------------------------------------------------------------------------

#define MOUSEFIX_CONFIG_FILE_NAME "mousefix.txt"

struct MouseFixSettings
{
	float mouse_x;     // VC:MP 0.4 equivalent: game_sensitivity
	float mouse_y;     // VC:MP 0.4 equivalent: game_sensitivity_ratio
	WORD  trigger_key; // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
};

// -----------------------------------------------------------------------------
