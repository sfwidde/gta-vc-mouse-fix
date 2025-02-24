/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#pragma once

#include <stdio.h>

// -----------------------------------------------------------------------------

#define CONFIGFILE_SUCCESS         0
#define CONFIGFILE_FILE_OPEN_ERROR 1

#define MAX_CONFIG_FILE_SETTING_NAME  64
#define MAX_CONFIG_FILE_SETTING_VALUE 256

class ConfigFile
{
//public:
//	static const char* GetErrorMessage(int errorCode);

private:
	FILE* file;
	//long  line;
	char  settingName[MAX_CONFIG_FILE_SETTING_NAME];
	char  settingValue[MAX_CONFIG_FILE_SETTING_VALUE];

public:
	int  Open(const char* fileName);
	void Close();

	bool  ReadNextLine();
	//long  GetLineNumber() const { return line; }
	char* GetSettingName() { return *settingName ? settingName : nullptr; }
	char* GetSettingValue() { return *settingValue ? settingValue : nullptr; }
};

// -----------------------------------------------------------------------------
