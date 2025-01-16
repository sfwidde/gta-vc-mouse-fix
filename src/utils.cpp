/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#include "utils.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>

// -----------------------------------------------------------------------------

void PatchMem(UINT_PTR address, PCVOID value, SIZE_T size)
{
	DWORD oldProt1, oldProt2;
	VirtualProtect((PVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProt1);
	memcpy((PVOID)address, value, size);
	VirtualProtect((PVOID)address, size, oldProt1, &oldProt2);
}

// -----------------------------------------------------------------------------

const char* ConfigFile::GetErrorMessage(int errorCode)
{
	static constexpr const char* messages[] =
	{
		"success",                          // CONFIGFILE_SUCCESS
		"unable to open configuration file" // CONFIGFILE_FILE_OPEN_ERROR
	};
	assert(errorCode >= 0 && errorCode <= (int)(GetArraySize(messages) - 1));
	return messages[errorCode];
}

int ConfigFile::Open(const char* fileName)
{
	file = fopen(fileName, "r");
	if (!file) { return CONFIGFILE_FILE_OPEN_ERROR; }

	line = 0;
	return CONFIGFILE_SUCCESS;
}

void ConfigFile::Close()
{
	fclose(file);
}

bool ConfigFile::ReadNextLine()
{
	// We cannot read any more data past this point
	if (feof(file)) { return false; }

	int c;
	char* p = settingName;
	while (true)
	{
		c = fgetc(file);
		// We are done with this line
		if (c == '\n' || c == EOF)
		{
			*p = '\0';
			*settingValue = '\0';
			++line;
			return true;
		}

		// Did we just encounter a whitespace character?
		if (isspace((unsigned char)c))
		{
			// Ignore leading whitespaces from setting name
			if (p == settingName) { continue; }

			// We do actually have a setting name, so null terminate it now
			*p = '\0';

			// From now on, any future characters we may encounter will be treated as part of the setting value
			p = settingValue;
			char* trailingWhitespacePos = nullptr;
			while (true)
			{
				c = fgetc(file);
				if (c == '\n' || c == EOF)
				{
					*(trailingWhitespacePos ? trailingWhitespacePos : p) = '\0';
					++line;
					return true;
				}

				// Make sure we don't overflow
				if ((p - settingValue) >= (MAX_CONFIG_FILE_SETTING_VALUE - 1)) { continue; }

				if (isspace((unsigned char)c))
				{
					// Ignore leading whitespaces from setting value
					if (p == settingValue) { continue; }

					// Mark this as the possible beginning of setting value's trailing whitespaces (if any)
					if (!trailingWhitespacePos) { trailingWhitespacePos = p; }
				}
				else { trailingWhitespacePos = nullptr; /* Nevermind */ }

				*p++ = c;
			}
		}

		// Store a new character into setting name only if there is enough room to do so
		if ((p - settingName) < (MAX_CONFIG_FILE_SETTING_NAME - 1))
		{
			*p++ = c;
		}
	}
}

// -----------------------------------------------------------------------------
