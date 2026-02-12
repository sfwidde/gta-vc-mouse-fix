/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: settings.cpp | Created: 2025-05-22
 * Author: sfwidde
 */

#include "settings.hpp"
#include "util/io.hpp"
#include "main.hpp"
#include "debug.hpp"
#include "util/common.hpp"
#include <string.h>
#include <stdlib.h>

#define SETTING_FLAG_NONE       ((DWORD)0)
#define SETTING_FLAG_MOUSE_X    ((DWORD)1)
#define SETTING_FLAG_MOUSE_Y    ((DWORD)1 << 1)
#define SETTING_FLAG_RELOAD_KEY ((DWORD)1 << 2)

void MouseFixSettings::ReadFromConfigFile()
{
	// Flags will help us acknowledge which settings
	// were indeed read from the config file and shall not be
	// overwritten by their corresponding default value
	DWORD flags = SETTING_FLAG_NONE;

	ConfigFile mousefix_txt;
	int x = mousefix_txt.Open(MOD_CONFIG_FILE_NAME);
	if (x == CONFIGFILE_SUCCESS)
	{
		const char* settingName;
		const char* settingValue;
		char* rem;
		OutputDebug("Retrieving user settings from '" MOD_CONFIG_FILE_NAME "'...");
		while (mousefix_txt.ReadNextLine())
		{
			settingName = mousefix_txt.GetSettingName();
			settingValue = mousefix_txt.GetSettingValue();
			if (!settingName || !settingValue) { continue; }

			if (!stricmp(settingName, "mouse_x"))
			{
				mouse_x = strtof(settingValue, &rem);
				if ((!*rem) && (mouse_x >= 0.0f))
				{
					ADD_FLAGS(flags, SETTING_FLAG_MOUSE_X);
					OutputDebug("-> mouse_x: %f", mouse_x);
				}
			}
			else if (!stricmp(settingName, "mouse_y"))
			{
				mouse_y = strtof(settingValue, &rem);
				if ((!*rem) && (mouse_y >= 0.0f))
				{
					ADD_FLAGS(flags, SETTING_FLAG_MOUSE_Y);
					OutputDebug("-> mouse_y: %f", mouse_y);
				}
			}
			else if (!stricmp(settingName, "reload_key"))
			{
				reload_key = (UINT)strtoul(settingValue, &rem, 16);
				if ((!*rem) &&
					(reload_key >= VK_LBUTTON) &&
					(reload_key <= VK_OEM_CLEAR) &&
					(reload_key != VK_F10))
				{
					ADD_FLAGS(flags, SETTING_FLAG_RELOAD_KEY);
					OutputDebug("-> reload_key: 0x%02X", reload_key);
				}
			}
		}
		mousefix_txt.Close();
	}
	else { OutputDebug(MOD_CONFIG_FILE_NAME ": %s.", ConfigFile::GetErrorMessage(x)); }

	/* Default values in case we failed to read any of the settings */
	if (!HAS_ANY_FLAGS(flags, SETTING_FLAG_MOUSE_X))
	{
		mouse_x = 0.0f;
		OutputDebug("-> mouse_x: %f (default)", mouse_x);
	}
	if (!HAS_ANY_FLAGS(flags, SETTING_FLAG_MOUSE_Y))
	{
		mouse_y = 1.2f;
		OutputDebug("-> mouse_y: %f (default)", mouse_y);
	}
	if (!HAS_ANY_FLAGS(flags, SETTING_FLAG_RELOAD_KEY))
	{
		reload_key = VK_F9;
		OutputDebug("-> reload_key: 0x%02X (default)", reload_key);
	}
}
