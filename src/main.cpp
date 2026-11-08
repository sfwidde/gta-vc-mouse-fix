/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: main.cpp | Created: 2024-01-10
 * Author: sfwidde (Kelvin)
 */

/*
	Many thanks =) to
	- Hanney (Mouse X and Y addresses)
	- Silent (Mouse lock fix based off his code) (https://github.com/CookiePLMonster/SilentPatch/blob/2a597da1bc2b974082c8b1fc13c08788b42615af/SilentPatchVC/SilentPatchVC.cpp#L2134)
	- AdTec_224 (CGame::Process() address)
 */

#include "main.hpp"
#include "patch.hpp"
#include "util/common.hpp"
#include "settings.hpp"
#include "debug.hpp"
#include <windows.h>

static FLOAT* g_gameMouseX = (FLOAT*)0x94DBD0;
static FLOAT* g_gameMouseY = (FLOAT*)0xA0D964;
static Hook   g_gameMenuOnRestoreHook(0x498F31, 7); // sub_498E5F
static Hook   g_gameProcessHook(0x4A4410, 7); // CGame::Process()

// -----------------------------------------------------------------------------
// HOOK PROCEDURES
// -----------------------------------------------------------------------------

static NAKED GameMenuOnRestoreHook()
{
	__asm pushad

	// Reload settings now
	MouseFixSettings::ReadFromConfigFile();

	if (g_gameMenuOnRestoreHook.m_firstHooked) {
		__asm popad
		__asm cmp byte ptr ds:0x697D54, 0
	} else {
		__asm popad
	}
	__asm jmp g_gameMenuOnRestoreHook.m_jmpAddress
}

static NAKED CGame_ProcessHook()
{
	__asm pushad

	// Don't question me, this is the (awfully awful) way VC:MP 0.4 has been doing it
	if (MouseFixSettings::mouse_x != 0.0f) { *g_gameMouseX = MouseFixSettings::mouse_x; }
	*g_gameMouseY = (*g_gameMouseX * MouseFixSettings::mouse_y);

	if (g_gameProcessHook.m_firstHooked) {
		__asm popad

		__asm push ebx
		__asm push ebp
		__asm mov  ecx, 0x4AB6C0
		__asm call ecx
	} else {
		__asm popad
	}
	__asm jmp g_gameProcessHook.m_jmpAddress
}

// -----------------------------------------------------------------------------

static void Init()
{
	// Initial settings setup
	MouseFixSettings::ReadFromConfigFile();

	Patch<BYTE[3]> gameMouseFix(0x601740);
	gameMouseFix.m_data[0] = 0x30; // xor al, al
	gameMouseFix.m_data[1] = 0xC0;
	gameMouseFix.m_data[2] = 0xC3; // ret

	g_gameMenuOnRestoreHook.Attach(GameMenuOnRestoreHook);
	g_gameProcessHook.Attach(CGame_ProcessHook);
}

static void Final()
{
	// Revert whatever changes we made
	g_gameProcessHook.Detach();
	g_gameMenuOnRestoreHook.Detach();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		OpenDebugConsole();
		Init();
		break;

	case DLL_PROCESS_DETACH:
		Final();
		CloseDebugConsole();
		break;
	}
	return TRUE;
}

// -----------------------------------------------------------------------------
