/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2025-01-28
 */

#pragma once

#include <windows.h>
#include <string.h>

template<typename T>
class GameMem
{
public:
	T& value; // Can be read from and written to
private:
	DWORD initialProt; // Original protection value

public:
	GameMem(DWORD address) : value(*(T*)address)
	{
		// Unprotect this chunk of memory until we are done working with it
		VirtualProtect(&value, sizeof(T), PAGE_EXECUTE_READWRITE, &initialProt);
	}

	~GameMem()
	{
		// Restore original protection value
		DWORD myProt;
		VirtualProtect(&value, sizeof(T), initialProt, &myProt);
	}
};

template<SIZE_T ExtraByteCount/* = 0*/>
void InstallGameFunctionHook(DWORD address, LPCVOID functionHook)
{
	// jmp (1) + address (4) = 5 + extra bytes
	GameMem<BYTE[5 + ExtraByteCount]> theHook(address); // Unprotect

	*theHook.value = 0xE9; // The JMP opcode
	*(DWORD*)(theHook.value + 1) = ((DWORD)functionHook - (address + 5)); // Redirect address
	memset((theHook.value + 5), 0x90, ExtraByteCount); // Fill extra bytes with NOPs

	// Re-protect upon object deletion
}
