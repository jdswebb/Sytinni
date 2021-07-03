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
#include "swg/misc/swg_math.h"
#include "swg/appearance/extent.h"
#include <functional>

namespace utinni
{

namespace Graphics
{
    extern UTINNI_API std::vector<std::function<void(float elapsedTime)>> preUpdateCallback;
    extern UTINNI_API std::vector<std::function<void(float elapsedTime)>> postUpdateCallback;
               
    extern UTINNI_API std::vector<std::function<void()>> preBeginSceneCallback;
    extern UTINNI_API std::vector<std::function<void()>> postBeginSceneCallback;
                                                       
    extern UTINNI_API std::vector<std::function<void()>> preEndSceneCallback;
    extern UTINNI_API std::vector<std::function<void()>> postEndSceneCallback;
             
    extern UTINNI_API std::vector<std::function<void(HWND hwnd, int width, int height)>> prePresentWindowCallback;
    extern UTINNI_API std::vector<std::function<void(HWND hwnd, int width, int height)>> postPresentWindowCallback;
               
    extern UTINNI_API std::vector<std::function<void()>> prePresentCallback;
    extern UTINNI_API std::vector<std::function<void()>> postPresentCallback;

    template<typename T>
    void addPreUpdateLoopCallback(T func)
    {
        preUpdateCallback.emplace_back(func);
    }

    template<typename T>
    void addPostUpdateLoopCallback(T func)
    {
        postUpdateCallback.emplace_back(func);
    }

    template<typename T>
    void addPreBeginSceneCallback(T func)
    {
        preBeginSceneCallback.emplace_back(func);
    }

    template<typename T>
    void addPostBeginSceneCallback(T func)
    {
        postBeginSceneCallback.emplace_back(func);
    }

    template<typename T>
    void addPreEndSceneCallback(T func)
    {
        preEndSceneCallback.emplace_back(func);
    }

    template<typename T>
    void addPostEndSceneCallback(T func)
    {
        postEndSceneCallback.emplace_back(func);
    }

    template<typename T>
    void addPrePresentWindowCallback(T func)
    {
        prePresentWindowCallback.emplace_back(func);
    }

    template<typename T>
    void addPostPresentWindowCallback(T func)
    {
        postPresentWindowCallback.emplace_back(func);
    }

    template<typename T>
    void addPrePresentCallback(T func)
    {
        prePresentCallback.emplace_back(func);
    }

    template<typename T>
    void addPostPresentCallback(void(*func)())
    {
        postPresentCallback.emplace_back(func);
    }

    
    UTINNI_API void useHardwareCursor(bool value);
    UTINNI_API void showMouseCursor(bool isShown);
    UTINNI_API void setSystemMouseCursorPosition(int X, int Y);
    
    UTINNI_API int getCurrentRenderTargetWidth();
    UTINNI_API int getCurrentRenderTargetHeight();
    
    UTINNI_API void flushResources(bool fullFlush);
    
    UTINNI_API void reloadTextures();
    
    UTINNI_API void setStaticShader(swgptr staticShader, int pass = 0);
    UTINNI_API void setObjectToWorldTransformAndScale(swg::math::Transform* objecToWorld, swg::math::Vector* scale);
    UTINNI_API void drawExtent(Extent* extent, swgptr vecArgbColor);
    
    UTINNI_API void detour();
};

};

