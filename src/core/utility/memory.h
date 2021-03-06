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

#include "utinni.h"

namespace memory
{
UTINNI_API extern swgptr findPattern(swgptr startAddress, size_t length, const char* pattern, const char* mask);
UTINNI_API extern swgptr findPattern(const char* moduleName, const char* pattern, const char* mask);

template<typename T>
extern T read(swgptr address)
{
    if (address > 0)
    {
        return *(T*)address;
    }
    return T();
}

template<typename T>
extern T read(swgptr address, int offsetOfAddrResult)
{
    const swgptr resultAddress = read<swgptr>(address);
    if (resultAddress > 0)
    {
        return *(T*)(resultAddress + offsetOfAddrResult);
    }
    return T();
}

template<typename T>
extern void write(swgptr address, T value)
{
    if (address > 0)
    {
        *(T*)address = value;
    }
}

template<typename T>
extern void write(swgptr address, int offsetOfAddrResult, T value)
{
    const swgptr resultAddress = read<swgptr>(address);
    if (resultAddress > 0)
    {
        *(T*)(resultAddress + offsetOfAddrResult) = value;
    }
}

UTINNI_API extern void copy(swgptr pDest, swgptr pSource, size_t length);

template<size_t n>
extern void copy(swgptr pDest, const unsigned char(&buffer)[n])
{
    copy(pDest, (swgptr)buffer, n);
}

template<typename T>
extern void copy(swgptr pDest, const std::vector<T>& vector)
{
    memory::copy(pDest, (swgptr)&vector[0], vector.size() * sizeof(T));
}

UTINNI_API extern void write(swgptr address, swgptr value, int length);
UTINNI_API extern void set(swgptr pDest, swgptr value, size_t length);

UTINNI_API extern void patchAddress(swgptr address, swgptr value);
UTINNI_API extern std::tuple<swgptr, std::vector<char>> nopAddress(swgptr address, int nopCount);
UTINNI_API extern void restoreBytes(const std::tuple <swgptr, std::vector<char>>& originalBytes);

UTINNI_API extern void createJMP(swgptr address, swgptr jumpToAddress, size_t overrideLength);

UTINNI_API extern swgptr getAddress(swgptr baseAddress, int ptrDepth);
UTINNI_API extern swgptr getAddress(swgptr baseAddress, std::vector<int>& offsets);

}

