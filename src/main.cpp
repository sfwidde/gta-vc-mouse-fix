/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: main.cpp | Created: 2024-01-10
 * Author: sfwidde
 */

/*
	Many thanks =) to
	- Swoorup, K^2 (WndProc subclass) (https://gtaforums.com/topic/490294-how-to-mod-gta-from-c/?do=findComment&comment=1060809319)
	- Silent (Mouse lock fix based off his code) (https://github.com/CookiePLMonster/SilentPatch/blob/2a597da1bc2b974082c8b1fc13c08788b42615af/SilentPatchVC/SilentPatchVC.cpp#L2134)
	- Hanney (Mouse X and Y addresses)
	- AdTec_224 (CGame::Process() address)
 */

#include "main.hpp"
#include "settings.hpp"
#include "gamepatch.hpp"
#include "util/common.hpp"
#include "debug.hpp"
#include <windows.h>

static LONG oldWndProc; // Original WndProc procedure before subclassing
static MouseFixSettings mouseFixSettings; // User settings loaded into memory
static GamePatch gameWnd(0x7897A4, 4); // HWND*
static GamePatch gameMouseFix(0x601740, 6); // sub_601740
static GamePatch gameMouseX(0x94DBD0, 4); // FLOAT*
static GamePatch gameMouseY(0xA0D964, 4); // FLOAT*
static GamePatch gameProcessHook(0x4A4410, 7); // CGame::Process()

// -----------------------------------------------------------------------------
// HOOK CALLBACKS
// -----------------------------------------------------------------------------

static void OnGameFrame()
{
	// Don't question me, this is the (awfully awful) way VC:MP 0.4 has been doing it
	FLOAT& x = *(FLOAT*)gameMouseX.m_address;
	FLOAT& y = *(FLOAT*)gameMouseY.m_address;
	if (mouseFixSettings.mouse_x != 0.0f) { x = mouseFixSettings.mouse_x; }
	y = (x * mouseFixSettings.mouse_y);
}

static void OnKeyDown(UINT key)
{
	// Reload settings now
	if (key == mouseFixSettings.reload_key) { mouseFixSettings.ReadFromConfigFile(); }
}

// -----------------------------------------------------------------------------
// HOOK PROCEDURES
// -----------------------------------------------------------------------------

static LRESULT CALLBACK WndProcSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		OnKeyDown((UINT)wParam);
	default:
		return CallWindowProc((WNDPROC)oldWndProc, hWnd, uMsg, wParam, lParam);
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
		push ebx
		push ebp
		mov  eax, 0x4AB6C0
		call eax

		; Resume execution at 0x4A4410 + 7
		mov eax, 0x4A4417
		jmp eax
	}
}

// -----------------------------------------------------------------------------

static void Init()
{
	// Initial settings setup
	mouseFixSettings.ReadFromConfigFile();

	// [xor al, al] + [ret]
	gameMouseFix.Install(3, 0x30, 0xC0, 0xC3);

	// Because unprotecting and reprotecting these on every frame
	// would be a dumb-ass move we do it only once right here
	gameMouseX.RemoveMemoryProtection();
	gameMouseY.RemoveMemoryProtection();

	// Hook CGame::Process()
	gameProcessHook.Install(CGame_ProcessHook);

	// Create a(nother - other DLLs could have done this themselves too) WndProc subclass
	gameWnd.RemoveMemoryProtection();
	oldWndProc = SetWindowLong(*(HWND*)gameWnd.m_address, GWL_WNDPROC, (LONG)WndProcSubclass);
	gameWnd.RestoreMemoryProtection();
}

static void Final()
{
	// Revert whatever changes we made

	gameWnd.RemoveMemoryProtection();
	SetWindowLong(*(HWND*)gameWnd.m_address, GWL_WNDPROC, oldWndProc);
	gameWnd.RestoreMemoryProtection();

	gameProcessHook.RestoreOldData();
	gameMouseY.RestoreMemoryProtection();
	gameMouseX.RestoreMemoryProtection();
	gameMouseFix.RestoreOldData();
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
