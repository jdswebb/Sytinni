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

#include "plugin_manager.h"
#include "utility/string_utility.h"
#include "utility/log.h"
#include "ini.h"

namespace utinni
{
    using pCreatePlugin = UtinniPlugin*(*)();

    struct PluginManager::Impl
    {
        std::vector<PluginConfig> pluginConfigs;
        std::vector<UtinniPlugin*> plugins;
    };

    PluginManager::PluginManager() : pImpl(new Impl) { }

    PluginManager::~PluginManager()
    {
        for (const auto& plugin : pImpl->plugins)
        {
            delete plugin;
        }

        delete pImpl;
    }

    void PluginManager::loadPlugins() const
    {
        auto& cfg = getConfig();

        std::string pluginsStr = cfg.getString("UtinniCore", "plugins");
        std::vector<std::string> plugins;
        std::istringstream f(pluginsStr);
        std::string s;
        while (std::getline(f, s, ',')) {
            plugins.push_back(s);
        }
       
        for (auto p : plugins)
        {
            log::info("Loading plugin:");
            log::info((p + ".dll").c_str());

            // Try to load the found .dll so that it that the createPlugin function can be looked up
            const HINSTANCE hDllInstance = LoadLibrary((getPath() + p + ".dll").c_str());
            if (hDllInstance != nullptr)
            {
                log::info("Plugin instance created");

                // Check if it is a valid UtinniPlugin, which contains the createPlugin function
                const auto createPlugin = (pCreatePlugin)GetProcAddress(hDllInstance, "createPlugin");
                if (createPlugin != nullptr)
                {
                    log::info("Plugin created");

                    // If the function exists in the dll, call the createPlugin function and emplace it in the plugin manager plugin list
                    UtinniPlugin* plugin = createPlugin();
                    if (plugin != nullptr)
                    {
                        log::info("Plugin added");
                        plugin->init();
                        pImpl->plugins.emplace_back(plugin);
                    }
                }
            }
        }
    }

    int PluginManager::getPluginConfigCount() const
    {
        return pImpl->pluginConfigs.size();
    }

    const PluginManager::PluginConfig& PluginManager::getPluginConfigAt(int i)
    {
        return pImpl->pluginConfigs.at(i);
    }
}
