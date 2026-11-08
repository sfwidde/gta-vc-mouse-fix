/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: patch.cpp | Created: 2025-04-01
 * Author: sfwidde (Kelvin)
 */

#include "patch.hpp"
#include "debug.hpp"
#include <assert.h>
#include <string.h>

// -----------------------------------------------------------------------------

// THIS STRUCT EXISTS MERELY FOR CONVENIENCE. IT DOES NOT CONSTITUTE AN ACTUAL OBJECT
// THUS FIELDS MUST BE CONTIGUOUS AND MUST NOT HAVE ANY PADDING BYTES IN BETWEEN. THIS IS FRAGILE
#pragma pack(push, 1)
struct HookData
{
	BYTE opcode; // 0-1
	DWORD address; // 1-5
};
#pragma pack(pop)

// -----------------------------------------------------------------------------

Hook::Hook(DWORD address, int byteCount)
{
	// jmp (1) + address (4) = 5 [+ remaining bytes]
	assert(byteCount >= 5 && byteCount <= MAX_HOOK_BYTES);

	m_address = address;
	m_byteCount = byteCount;
}

void Hook::Attach(LPCVOID functionHook)
{
	// *We* only care about the first 5 bytes, but the compiler
	// won't give a shit about that as long as we provide
	// a fucking pointer to VirtualProtect and mem* functions 3:)
	HookData* data = (HookData*)m_address;

	// This step is veeeeeery important
	DWORD oldProt1, oldProt2;
	VirtualProtect(data, m_byteCount, PAGE_EXECUTE_READWRITE, &oldProt1);

	// Preserve original bytes before performing a meticulous surgery
	memcpy(m_oldBytes, data, m_byteCount);

	// Is a (presumably) hook already present at this address?
	if (data->opcode == JMP)
	{
		m_jmpAddress = m_address + 5 + data->address;
		m_firstHooked = false;
	}
	// We are the first ones hooking to this address
	else
	{
		m_jmpAddress = m_address + m_byteCount;
		m_firstHooked = true;
		data->opcode = JMP;
	}
	data->address = ((DWORD)functionHook - (m_address + 5)); // Redirect address
	memset(data + 1, NOP, m_byteCount - 5); // nop remaining bytes. +1 advances by 5 bytes

	// Close the wound
	VirtualProtect(data, m_byteCount, oldProt1, &oldProt2);

	OutputDebug("Attached a %d-byte hook to address 0x%lX.", m_byteCount, m_address);
}

void Hook::Detach()
{
	DWORD oldProt1, oldProt2;
	VirtualProtect((PVOID)m_address, m_byteCount, PAGE_EXECUTE_READWRITE, &oldProt1);
	memcpy((PVOID)m_address, m_oldBytes, m_byteCount);
	VirtualProtect((PVOID)m_address, m_byteCount, oldProt1, &oldProt2);

	OutputDebug("Detached a %d-byte hook from address 0x%lX.", m_byteCount, m_address);
}

// -----------------------------------------------------------------------------
