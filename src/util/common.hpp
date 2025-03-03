/*
 * Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
 * Author: sfwidde ([SS]Kelvin)
 * 2024-01-10
 */

#pragma once

//#include <stddef.h>

#define NAKED __declspec(naked) void

// https://stackoverflow.com/a/18078435/14197165
//template<typename T, size_t N>
//constexpr size_t GetArraySize(const T (&)[N]) { return N; }
