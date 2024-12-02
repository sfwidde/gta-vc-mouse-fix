/*
 * Mouse sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

#define CONFIGFILE_SUCCESS         0
#define CONFIGFILE_ALLOC_ERROR     1
#define CONFIGFILE_FILE_OPEN_ERROR 2

#define MAX_CONFIG_FILE_SETTING_NAME  64
#define MAX_CONFIG_FILE_SETTING_VALUE 256

typedef struct ConfigFile ConfigFile;

int         OpenConfigFile(ConfigFile** file, const char* fileName);
void        CloseConfigFile(ConfigFile* file);
bool        ReadNextConfigFileLine(ConfigFile* file);
//long        GetConfigFileLine(const ConfigFile* file);
char*       GetConfigFileSettingName(ConfigFile* file);
char*       GetConfigFileSettingValue(ConfigFile* file);
//const char* GetConfigFileErrorMessage(int errorCode);

// -----------------------------------------------------------------------------

void ModifyGTAMemory(void* address, const void* value, size_t size);

#endif
