/*
 * Mouse sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
//#include <assert.h>
#include <Windows.h>

struct ConfigFile
{
	FILE* file;
	//long  line;
	char  settingName[MAX_CONFIG_FILE_SETTING_NAME];
	char  settingValue[MAX_CONFIG_FILE_SETTING_VALUE];
};

int OpenConfigFile(ConfigFile** file, const char* fileName)
{
	ConfigFile* newConfigFile = malloc(sizeof(ConfigFile));
	if (!newConfigFile)
	{
		return CONFIGFILE_ALLOC_ERROR;
	}

	newConfigFile->file = fopen(fileName, "r");
	if (!newConfigFile->file)
	{
		free(newConfigFile);
		return CONFIGFILE_FILE_OPEN_ERROR;
	}

	//newConfigFile->line = 0;
	*file = newConfigFile;
	return CONFIGFILE_SUCCESS;
}

void CloseConfigFile(ConfigFile* file)
{
	fclose(file->file);
	free(file);
}

bool ReadNextConfigFileLine(ConfigFile* file)
{
	// Have we reached the end of the file already?
	if (feof(file->file))
	{
		// We cannot read any more data past this point.
		return false;
	}

	int c;
	char* p = file->settingName;
	while (true)
	{
		c = fgetc(file->file);
		// We are done with this line.
		if (c == '\n' || c == EOF)
		{
			*p = '\0';
			*file->settingValue = '\0';
			//++file->line;
			return true;
		}

		// Did we just encounter a whitespace character?
		if (isspace((unsigned char)c))
		{
			// Ignore leading whitespaces from setting name.
			if (p == file->settingName) { continue; }

			// We do actually have a setting name, so null terminate it now.
			*p = '\0';

			// From now on, any future characters we may encounter will be treated as part of the setting value.
			p = file->settingValue;
			char* trailingWhitespacePos = NULL;
			while (true)
			{
				c = fgetc(file->file);
				if (c == '\n' || c == EOF)
				{
					*(trailingWhitespacePos ? trailingWhitespacePos : p) = '\0';
					//++file->line;
					return true;
				}

				// Make sure we won't overflow.
				if ((p - file->settingValue) >= (MAX_CONFIG_FILE_SETTING_VALUE - 1))
				{
					continue;
				}

				if (isspace((unsigned char)c))
				{
					// Ignore leading whitespaces from setting value.
					if (p == file->settingValue) { continue; }

					// Mark this as the possible beginning of setting value's trailing whitespaces (if any).
					if (!trailingWhitespacePos) { trailingWhitespacePos = p; }
				}
				else { trailingWhitespacePos = NULL; /* Nevermind. */ }

				*p++ = c;
			}
		}

		// Store a new character into setting name only if there is enough room to do so.
		if ((p - file->settingName) < (MAX_CONFIG_FILE_SETTING_NAME - 1))
		{
			*p++ = c;
		}
	}
}

//long GetConfigFileLine(const ConfigFile* file)
//{
//	return file->line;
//}

char* GetConfigFileSettingName(ConfigFile* file)
{
	return *file->settingName ? file->settingName : NULL;
}

char* GetConfigFileSettingValue(ConfigFile* file)
{
	return *file->settingValue ? file->settingValue : NULL;
}

//const char* GetConfigFileErrorMessage(int errorCode)
//{
//	static const char* const messages[] =
//	{
//		"success",                                       // CONFIGFILE_SUCCESS
//		"unable to allocate a new ConfigFile structure", // CONFIGFILE_ALLOC_ERROR
//		"unable to open configuration file"              // CONFIGFILE_FILE_OPEN_ERROR
//	};
//	assert(errorCode >= 0 && errorCode <= (ARRAY_SIZE(messages) - 1));
//	return messages[errorCode];
//}

// -----------------------------------------------------------------------------

void ModifyGTAMemory(void* address, const void* value, size_t size)
{
	DWORD oldProt1, oldProt2;
	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProt1);
	memcpy(address, value, size);
	VirtualProtect(address, size, oldProt1, &oldProt2);
}
