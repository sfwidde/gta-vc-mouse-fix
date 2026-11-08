/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: patch.hpp | Created: 2025-01-28
 * Author: sfwidde (Kelvin)
 */

#pragma once

#include "debug.hpp"
#include <windows.h>

#define NOP 0x90
#define JMP 0xE9

// -----------------------------------------------------------------------------

template<typename T>
class Patch
{
public:
	T& m_data;
private:
	DWORD m_oldProt;

public:
	Patch(DWORD address);
	~Patch();
};

template<typename T>
Patch<T>::Patch(DWORD address) : m_data(*(T*)address)
{
	VirtualProtect(&m_data, sizeof(T), PAGE_EXECUTE_READWRITE, &m_oldProt);
	OutputDebug("Removed %zu bytes of memory protection at address 0x%lX.", sizeof(T), (DWORD)&m_data);
}

template<typename T>
Patch<T>::~Patch()
{
	DWORD myProt;
	VirtualProtect(&m_data, sizeof(T), m_oldProt, &myProt);
	OutputDebug("Restored %zu bytes of memory protection at address 0x%lX.", sizeof(T), (DWORD)&m_data);
}

// -----------------------------------------------------------------------------

// Do increase on demand..!
#define MAX_HOOK_BYTES 7

class Hook
{
private:
	DWORD m_address; // Install address
public:
	DWORD m_jmpAddress; // Address to jump back to
private:
	int   m_byteCount; // Bytes to patch in total
public:
	bool  m_firstHooked; // Was this address hooked for the first time?
private:
	BYTE  m_oldBytes[MAX_HOOK_BYTES]; // Original bytes before patching

public:
	Hook(DWORD address, int byteCount);
	void Attach(LPCVOID functionHook);
	void Detach();
};

// -----------------------------------------------------------------------------
