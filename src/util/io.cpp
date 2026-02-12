/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: io.cpp | Created: 2024-01-10
 * Author: sfwidde
 */

#include "io.hpp"
#include "common.hpp"
#include <assert.h>
#include <ctype.h>

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
	m_file = fopen(fileName, "r");
	if (!m_file) { return CONFIGFILE_FILE_OPEN_ERROR; }

	m_lineNumber = 0;
	return CONFIGFILE_SUCCESS;
}

void ConfigFile::Close()
{
	fclose(m_file);
}

bool ConfigFile::ReadNextLine()
{
	// We cannot read any more data past this point
	if (feof(m_file)) { return false; }

	int c;
	char* p = m_settingName;
	while (true)
	{
		c = fgetc(m_file);
		// We are done with this line
		if ((c == '\n') || (c == EOF))
		{
		lineDone:
			*p = '\0'; // m_settingName
			*m_settingValue = '\0';
			++m_lineNumber;
			return true;
		}

		// Line begins with a ';' and should therefore be treated as a comment
		if ((c == ';') && (p == m_settingName))
		{
			// Ignore all following characters until a newline is found
			do { c = fgetc(m_file); } while ((c != '\n') && (c != EOF));
			goto lineDone;
		}

		// We have just encountered a whitespace character
		if (isspace((unsigned char)c))
		{
			// Ignore leading whitespaces from setting name
			if (p == m_settingName) { continue; }

			// We do actually have a setting name, so null terminate it now
			*p = '\0';

			// From now on, any future characters we may encounter
			// will be treated as part of the setting value
			p = m_settingValue;
			char* trailingWsPos = nullptr;
			while (true)
			{
				c = fgetc(m_file);
				if ((c == '\n') || (c == EOF)) // END OF LINE
				{
					*(trailingWsPos ? trailingWsPos : p) = '\0';
					++m_lineNumber;
					return true;
				}

				// Make sure we don't overflow
				if ((p - m_settingValue) >= (MAX_CONFIG_FILE_SETTING_VALUE - 1)) { continue; }

				if (isspace((unsigned char)c))
				{
					// Ignore leading whitespaces from setting value
					if (p == m_settingValue) { continue; }

					// Mark this as the possible beginning of setting value's
					// trailing whitespaces (if any)
					if (!trailingWsPos) { trailingWsPos = p; }
				}
				else { trailingWsPos = nullptr; /* Nevermind */ }

				*p++ = c;
			}
		}

		// Store a new character into setting name only if there is enough room to do so
		if ((p - m_settingName) < (MAX_CONFIG_FILE_SETTING_NAME - 1)) { *p++ = c; }
	}
}

// -----------------------------------------------------------------------------
