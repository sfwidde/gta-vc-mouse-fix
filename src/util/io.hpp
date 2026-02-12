/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: io.hpp | Created: 2024-01-10
 * Author: sfwidde
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
public:
	static const char* GetErrorMessage(int errorCode);

private:
	FILE* m_file;
	long  m_lineNumber;
	char  m_settingName[MAX_CONFIG_FILE_SETTING_NAME];
	char  m_settingValue[MAX_CONFIG_FILE_SETTING_VALUE];

public:
	int  Open(const char* fileName);
	void Close();

	bool  ReadNextLine();
	long  GetLineNumber() const { return m_lineNumber; }
	char* GetSettingName() { return *m_settingName ? m_settingName : nullptr; }
	char* GetSettingValue() { return *m_settingValue ? m_settingValue : nullptr; }
};

// -----------------------------------------------------------------------------
