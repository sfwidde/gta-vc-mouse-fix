/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: gamepatch.cpp | Created: 2025-04-01
 * Author: sfwidde
 */

#include "gamepatch.hpp"
#include "debug.hpp"
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

GamePatch::GamePatch(DWORD address, int byteCount)
{
	assert((byteCount >= 1) && (byteCount <= MAX_GAME_PATCH_BYTES));

	m_address = address;
	m_byteCount = byteCount;
	m_hasOldData = false;
}

void GamePatch::RemoveMemoryProtection()
{
	VirtualProtect((PVOID)m_address, m_byteCount, PAGE_EXECUTE_READWRITE, &m_oldProt);
	if (!m_hasOldData)
	{
		memcpy(m_oldData, (LPCVOID)m_address, m_byteCount);
		m_hasOldData = true;
	}
	OutputDebug("Removed %d bytes of memory protection at address 0x%lX.", m_byteCount, m_address);
}

void GamePatch::RestoreMemoryProtection()
{
	DWORD myProt;
	VirtualProtect((PVOID)m_address, m_byteCount, m_oldProt, &myProt);
	OutputDebug("Restored %d bytes of memory protection at address 0x%lX.", m_byteCount, m_address);
}

void GamePatch::Install(int byteCount, ...)
{
	assert((byteCount >= 0) && (byteCount <= m_byteCount));

	RemoveMemoryProtection();
#ifdef _DEBUG
	char list[MAX_GAME_PATCH_BYTES * 3 + 1];
	int x = 0;
#endif
	va_list ap;
	va_start(ap, byteCount);
	for (int i = 0; i < m_byteCount; ++i)
	{
		BYTE& b = *(BYTE*)(m_address + i);
		b = (i < byteCount) ? (BYTE)va_arg(ap, int) : 0x90;
	#ifdef _DEBUG
		x += sprintf((list + x), " %02hhX", b);
	#endif
	}
	va_end(ap);
	OutputDebug("Installed a sequence of %d bytes at address 0x%lX:%s", m_byteCount, m_address, list);
	RestoreMemoryProtection();
}

void GamePatch::Install(GameHook functionHook)
{
	// jmp (1) + address (4) = 5 [+ remaining bytes]
	assert(m_byteCount >= 5);
	DWORD jmpAddress = ((DWORD)functionHook - (m_address + 5));
	Install(5, 0xE9,
		((BYTE*)&jmpAddress)[0],
		((BYTE*)&jmpAddress)[1],
		((BYTE*)&jmpAddress)[2],
		((BYTE*)&jmpAddress)[3]);
}

void GamePatch::RestoreOldData()
{
	RemoveMemoryProtection();
	memcpy((PVOID)m_address, m_oldData, m_byteCount);
	RestoreMemoryProtection();
}
