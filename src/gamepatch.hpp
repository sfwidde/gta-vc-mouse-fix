/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2025-01-28
 */

#pragma once

#include <windows.h>

//#define ASM_NOP 0x90
//#define ASM_JMP 0xE9

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
