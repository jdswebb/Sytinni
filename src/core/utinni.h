/**
 * MIT License
 *
 * Copyright (c) 2020 Philip Klatt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#pragma once

#include <cstdint>

#ifdef _MSC_VER
#pragma warning(disable : 4251) // inconsistent dll linkage
#pragma warning(disable : 4648)
#pragma warning(disable : 4996) // crt insecure warnings
#endif

using swgptr = uint32_t;
using byte = uint8_t;

#ifdef BUILDING_CORE
#define UTINNI_API __declspec(dllexport)
#define IMGUI_API  __declspec(dllexport)
#else
#define UTINNI_API __declspec(dllimport)
#define IMGUI_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "DetourXS/detourxs.h"

namespace utinni
{
class PluginManager;
}

namespace utinni
{
class UtINI;

UTINNI_API extern const std::string& getPath();
UTINNI_API extern const std::string& getSwgCfgFilename();
UTINNI_API extern UtINI& getConfig();
UTINNI_API extern PluginManager& getPluginManager();
};