/*
 * Mouse Lock Fix and Sensitivity Adjuster
 * for Grand Theft Auto: Vice City
 *
 * File: common.hpp | Created: 2024-01-10
 * Author: sfwidde
 */

#pragma once

#include <stddef.h>

// GOOD GRIEF, HE'S NAKED!!!!
#define NAKED __declspec(naked) void

#define ADD_FLAGS(x, flags)     ((x) |= (flags))
#define CLEAR_FLAGS(x, flags)   ((x) &= ~(flags))
#define HAS_ANY_FLAGS(x, flags) (!!((x) & (flags)))
#define HAS_ALL_FLAGS(x, flags) (((x) & (flags)) == (flags))

// https://stackoverflow.com/a/18078435/14197165
template<typename T, size_t N>
constexpr size_t GetArraySize(const T (&)[N]) { return N; }
