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
#include "swg/misc/repository.h"
#include <functional>

namespace utinni
{
class Camera;
class Object;

namespace Game
{
    extern UTINNI_API std::vector<std::function<void()>> installCallbacks;
    extern UTINNI_API std::vector<std::function<void()>> preMainLoopCallbacks;
    extern UTINNI_API std::vector<std::function<void()>> mainLoopCallbacks;
    extern UTINNI_API std::vector<std::function<void()>> setSceneCallbacks;
    extern UTINNI_API std::vector<std::function<void()>> cleanUpSceneCallbacks;

    template<typename T>
    void addInstallCallback(T func)
    {
        installCallbacks.emplace_back(func);
    }

    template<typename T>
    void addPreMainLoopCallback(T func)
    {
        preMainLoopCallbacks.emplace_back(func);
    }

    template<typename T>
    void addMainLoopCallback(T func)
    {
        mainLoopCallbacks.emplace_back(func);
    }

    template<typename T>
    void addSetSceneCallback(T func)
    {
        setSceneCallbacks.emplace_back(func);
    }

    template<typename T>
    void addCleanupSceneCallback(T func)
    {
        cleanUpSceneCallbacks.emplace_back(func);
    }
        
    UTINNI_API void detour();
    UTINNI_API void quit();

    UTINNI_API bool isRunning();

    UTINNI_API void loadScene();
    UTINNI_API void loadScene(const char* terrainFilename, const char* avatarObjectFilename = "object/creature/player/shared_human_male.iff");
    UTINNI_API void cleanupScene();

    UTINNI_API Repository*  getRepository();

    UTINNI_API Object* getPlayer();
    UTINNI_API Object* getPlayerCreatureObject();
    UTINNI_API swgptr getPlayerLookAtTargetObjectNetworkId();
    UTINNI_API Object* getPlayerLookAtTargetObject();

    UTINNI_API Camera* getCamera();
    UTINNI_API const Camera*  getConstCamera();

    UTINNI_API bool isSafeToUse();
};
}
