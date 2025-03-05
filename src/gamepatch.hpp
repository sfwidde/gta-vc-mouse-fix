/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2025-01-28
 */

#pragma once

#include <windows.h>
#include <string.h>

/*
	T = data type to interpret this address as
	AutoProtect = manual or automatic protection managing
 */
template<typename T, bool AutoProtect = true>
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
		if constexpr (AutoProtect) { Unprotect(); }
	}

	~GameMem()
	{
		// Restore original protection value
		if constexpr (AutoProtect) { Reprotect(); }
	}

	void Unprotect() { VirtualProtect(&value, sizeof(T), PAGE_EXECUTE_READWRITE, &initialProt); }
	void Reprotect() { DWORD myProt; VirtualProtect(&value, sizeof(T), initialProt, &myProt);   }
};

template<SIZE_T HookByteCount>
class GameHook
{
	// jmp (1) + address (4) = 5 [+ extra bytes]
	static_assert(HookByteCount >= 5);

private:
	DWORD address;

public:
	GameHook(DWORD address) : address(address) {}

	void Install(LPCVOID functionHook)
	{
		// Unprotect
		GameMem<BYTE[HookByteCount]> theHook(address);

		// Install the hook
		*theHook.value = 0xE9;                                                // The JMP opcode
		*(DWORD*)(theHook.value + 1) = ((DWORD)functionHook - (address + 5)); // Redirect address
		memset((theHook.value + 5), 0x90, (HookByteCount - 5));               // Fill extra bytes with NOPs

		// Re-protect upon object deletion
	}

	void Uninstall(const BYTE (&originalBytes)[HookByteCount])
	{
		// Restore original bytes
		memcpy(GameMem<BYTE[HookByteCount]>(address).value, originalBytes, HookByteCount);
	}
};
