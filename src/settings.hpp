/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: settings.hpp | Created: 2025-05-22
 * Author: sfwidde
 */

#pragma once

#include <windows.h>

class MouseFixSettings
{
public:
	float mouse_x;    // VC:MP 0.4 equivalent: game_sensitivity
	float mouse_y;    // VC:MP 0.4 equivalent: game_sensitivity_ratio
	UINT  reload_key; // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

	void ReadFromConfigFile();
};
