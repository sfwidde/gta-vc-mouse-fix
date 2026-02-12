/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: debug.hpp | Created: 2025-04-09
 * Author: sfwidde
 */

#pragma once

#ifdef _DEBUG
void OpenDebugConsole();
void CloseDebugConsole();
void OutputDebug(const char* format, ...);
#else
#define OpenDebugConsole()       ((void)0)
#define CloseDebugConsole()      ((void)0)
#define OutputDebug(format, ...) ((void)0)
#endif
