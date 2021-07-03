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

#include "graphics.h"
#include "swg/scene/ground_scene.h"
#include "swg/client/client.h"
#include "swg/ui/cui_manager.h"
#include "utility/utility.h"
#include "utility/memory.h"
#include "utility/log.h"
#include "directx9.h"

namespace swg::graphics
{
using pInstall = bool(__cdecl*)();

using pUpdate = void(__cdecl*)(float elapsedTime);
using pBeginScene = void(__cdecl*)();
using pEndScene = void(__cdecl*)();

using pPresentWindow = int(__cdecl*)(HWND hwnd, int width, int height);
using pPresent = void(__cdecl*)();

using pUseHardwareCursor = bool(__cdecl*)(bool value);
using pShowMouseCursor = bool(__cdecl*)(bool isShown);
using pSetSystemMouseCursorPosition = void(__cdecl*)(int X, int Y);

using pResize = void(__cdecl*)(int width, int height);
using pFlushResources = void(__cdecl*)(bool reset);

using pTextureListReloadTextures = void(__cdecl*)();

using pSetStaticShader = void(__cdecl*)(swgptr staticShader, int pass);
using pSetObjectToWorldTransformAndScale = void(__cdecl*)(math::Transform* objecToWorld, math::Vector* scale);
using pDrawExtent = void(__cdecl*)(utinni::Extent* extent, swgptr vecArgbColor);

using pScreenshot = bool(__cdecl*)(const char* filename);

pInstall install = (pInstall)0x007548A0;

pUpdate update = (pUpdate)0x00755700; 
pBeginScene beginScene = (pBeginScene)0x00755730; 
pEndScene endScene = (pEndScene)0x00755740;

pPresentWindow presentWindow = (pPresentWindow)0x00755810;
pPresent present = (pPresent)0x00755800; 

pUseHardwareCursor useHardwareCursor = (pUseHardwareCursor)0x00755940;
pShowMouseCursor showMouseCursor = (pShowMouseCursor)0x00755A50;
pSetSystemMouseCursorPosition setSystemMouseCursorPosition = (pSetSystemMouseCursorPosition)0x00755AC0;

pResize resize = (pResize)0x00754E40;
pFlushResources flushResources = (pFlushResources)0x00755520;

pTextureListReloadTextures textureListReloadTextures = (pTextureListReloadTextures)0x00764B70;

pSetStaticShader setStaticShader = (pSetStaticShader)0x00755910;
pSetObjectToWorldTransformAndScale setObjectToWorldTransformAndScale = (pSetObjectToWorldTransformAndScale)0x00755D30;
pDrawExtent drawExtent = (pDrawExtent)0x00759A70;

pScreenshot screenshot = (pScreenshot)0x00755890;
}

namespace utinni
{

namespace Graphics
{
std::string screenshotsDir = "screenshots/";

std::vector<std::function<void(float elapsedTime)>> preUpdateCallback;
std::vector<std::function<void(float elapsedTime)>> postUpdateCallback;

std::vector<std::function<void()>> preBeginSceneCallback;
std::vector<std::function<void()>> postBeginSceneCallback;

std::vector<std::function<void()>> preEndSceneCallback;
std::vector<std::function<void()>> postEndSceneCallback;

std::vector<std::function<void(HWND hwnd, int width, int height)>> prePresentWindowCallback;
std::vector<std::function<void(HWND hwnd, int width, int height)>> postPresentWindowCallback;

std::vector<std::function<void()>> prePresentCallback;
std::vector<std::function<void()>> postPresentCallback;
}

void Graphics::useHardwareCursor(bool value)
{
    swg::graphics::useHardwareCursor(value);
}

void Graphics::showMouseCursor(bool isShown)
{
    swg::graphics::showMouseCursor(isShown);
}

void Graphics::setSystemMouseCursorPosition(int X, int Y)
{
    swg::graphics::setSystemMouseCursorPosition(X, Y);
}

int Graphics::getCurrentRenderTargetWidth() // 0x00754DB0 is the clients function address
{
    return memory::read<int>(0x1922E64); // static ptr to RenderTargetWidth
}

int Graphics::getCurrentRenderTargetHeight() // 0x00754DC0 is the clients function address
{
    return memory::read<int>(0x1922E60); // static ptr to RenderTargetHeight
}

void Graphics::flushResources(bool fullFlush)
{
    swg::graphics::flushResources(fullFlush);
}

void Graphics::reloadTextures()
{
    swg::graphics::textureListReloadTextures();
}

void Graphics::setStaticShader(swgptr staticShader, int pass)
{
    swg::graphics::setStaticShader(staticShader, pass);
}

void Graphics::setObjectToWorldTransformAndScale(swg::math::Transform* objecToWorld, swg::math::Vector* scale)
{
    swg::graphics::setObjectToWorldTransformAndScale(objecToWorld, scale);
}

void Graphics::drawExtent(Extent* extent, swgptr vecArgbColor)
{
    swg::graphics::drawExtent(extent, vecArgbColor);
}

bool __cdecl hkInstall()
{
    bool result = swg::graphics::install();

    directX::detour();

    return result;
}

void __cdecl hkUpdate(float elapsedTime)
{
    for (const auto& func : Graphics::preUpdateCallback)
    {
        func(elapsedTime);
    }

    swg::graphics::update(elapsedTime);

    for (const auto& func : Graphics::postUpdateCallback)
    {
        func(elapsedTime);
    }
}

void __cdecl hkBeginScene()
{
    for (const auto& func : Graphics::preBeginSceneCallback)
    {
        func();
    }

    swg::graphics::beginScene();

    for (const auto& func : Graphics::postBeginSceneCallback)
    {
        func();
    }
}

int oldWidth = 0;
int oldHeight = 0;
void __cdecl hkEndScene()
{
    for (const auto& func : Graphics::preEndSceneCallback)
    {
        func();
    }

    swg::graphics::endScene();

    for (const auto& func : Graphics::postEndSceneCallback)
    {
        func();
    }
}

void __cdecl hkPresentWindow(HWND hwnd, int width, int height)
{
    for (const auto& func : Graphics::prePresentWindowCallback)
    {
        func(hwnd, width, height);
    }

    swg::graphics::presentWindow(hwnd, width, height);

    for (const auto& func : Graphics::postPresentWindowCallback)
    {
        func(hwnd, width, height);
    }
}

void __cdecl hkPresent()
{
    for (const auto& func : Graphics::prePresentCallback)
    {
        func();
    }

    swg::graphics::present();

    for (const auto& func : Graphics::postPresentCallback)
    {
        func();
    }
}

bool __cdecl hkScreenshot(const char* filename)
{
    std::string newFilename = Graphics::screenshotsDir;

    const char* pos = strrchr(filename, '/');
    if (pos != nullptr)
    {
        newFilename += pos + 1;
    }
    else
    {
        newFilename += filename;
    }

    CreateDirectory((utility::getWorkingDirectory() + "/" + Graphics::screenshotsDir).c_str(), nullptr);

    return swg::graphics::screenshot(newFilename.c_str());
}

void Graphics::detour()
{
    swg::graphics::install = (swg::graphics::pInstall)Detour::Create(swg::graphics::install, hkInstall, DETOUR_TYPE_PUSH_RET);

    swg::graphics::update = (swg::graphics::pUpdate)Detour::Create(swg::graphics::update, hkUpdate, DETOUR_TYPE_PUSH_RET);
    swg::graphics::beginScene = (swg::graphics::pBeginScene)Detour::Create(swg::graphics::beginScene, hkBeginScene, DETOUR_TYPE_JMP, 5);
    swg::graphics::endScene = (swg::graphics::pEndScene)Detour::Create(swg::graphics::endScene, hkEndScene, DETOUR_TYPE_JMP, 5);

    swg::graphics::presentWindow = (swg::graphics::pPresentWindow)Detour::Create(swg::graphics::presentWindow, hkPresentWindow, DETOUR_TYPE_JMP, 5);
    swg::graphics::present = (swg::graphics::pPresent)Detour::Create(swg::graphics::present, hkPresent, DETOUR_TYPE_JMP, 5);

    swg::graphics::screenshot = (swg::graphics::pScreenshot)Detour::Create(swg::graphics::screenshot, hkScreenshot, DETOUR_TYPE_PUSH_RET);
}

}

