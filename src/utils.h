/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#pragma once

#include <windows.h>
#include <stddef.h>
#include <stdio.h>

// -----------------------------------------------------------------------------

typedef CONST void* PCVOID; // To not use LPCVOID

// https://stackoverflow.com/a/18078435/14197165
template<typename T, size_t N>
constexpr size_t GetArraySize(const T(&)[N]) { return N; }

// -----------------------------------------------------------------------------

//#define NOP 0x90

void PatchMem(UINT_PTR address, PCVOID value, SIZE_T size);

// -----------------------------------------------------------------------------

#define CONFIGFILE_SUCCESS         0
#define CONFIGFILE_FILE_OPEN_ERROR 1

#define MAX_CONFIG_FILE_SETTING_NAME  64
#define MAX_CONFIG_FILE_SETTING_VALUE 256

class ConfigFile
{
public:
	static const char* GetErrorMessage(int errorCode);

private:
	FILE* file;
	long  line;
	char  settingName[MAX_CONFIG_FILE_SETTING_NAME];
	char  settingValue[MAX_CONFIG_FILE_SETTING_VALUE];

public:
	int  Open(const char* fileName);
	void Close();

	bool  ReadNextLine();
	long  GetLineNumber() const { return line; }
	char* GetSettingName() { return *settingName ? settingName : nullptr; }
	char* GetSettingValue() { return *settingValue ? settingValue : nullptr; }
};

// -----------------------------------------------------------------------------
