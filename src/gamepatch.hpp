/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: gamepatch.hpp | Created: 2025-01-28
 * Author: sfwidde
 */

#pragma once

#include <windows.h>

// Do increase on demand..!
#define MAX_GAME_PATCH_BYTES 7

typedef void (*GameHook)();

class GamePatch
{
public:
	DWORD  m_address;
private:
	DWORD  m_oldProt;
	int    m_byteCount;
	bool   m_hasOldData;
	BYTE   m_oldData[MAX_GAME_PATCH_BYTES];

public:
	GamePatch(DWORD address, int byteCount);

	void RemoveMemoryProtection();
	void RestoreMemoryProtection();

	void Install(int byteCount = 0, ...);
	void Install(GameHook functionHook);
	void RestoreOldData();
};
