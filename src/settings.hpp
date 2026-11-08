/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: settings.hpp | Created: 2025-05-22
 * Author: sfwidde (Kelvin)
 */

#pragma once

class MouseFixSettings
{
public:
	static float mouse_x; // VC:MP 0.4 equivalent: game_sensitivity
	static float mouse_y; // VC:MP 0.4 equivalent: game_sensitivity_ratio

	static void ReadFromConfigFile();
};
