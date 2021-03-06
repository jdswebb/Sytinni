-- build adapted from Astris (https://astris.dev/)
local SRC_ROOT = "../src/"
local DATA_ROOT = "../data"
local SYTINNI_ROOT = "../src/"
local PLUGINS_ROOT = "../src/plugins"
local EXT_ROOT = "../external/"
local SOL_NAME = "sytinni"
local SOL_EXT_DIR = "$(SolutionDir)..\\..\\external\\"
local SOL_DATA_DIR = "$(SolutionDir)..\\..\\data"
local SOL_BUILD_DIR = "$(SolutionDir)..\\..\\build\\"
local SOL_BIN_DIR = "$(SolutionDir)bin\\"
local PROJ_DIR = "generated/"
local TARGET_DIR = PROJ_DIR .. "bin/"
local VS_VER = "vs2019"

function commonDefines()
    defines {
        "WIN32_LEAN_AND_MEAN",
        "NOMINMAX",
        "SPDLOG_NO_EXCEPTION",
        "CONFIG_NAME=\"$(ConfigurationName)\"",
        "PROJECT_NAME=\"$(ProjectName)\"",
    }
end
function commonLinks()
    -- windows libs
    links { 
        "dxguid",
        "imm32",
        "psapi",
        "version",
        "winmm"
    }
end
function commonFlags()
    -- common flags
    flags {
        "FatalWarnings",
        "NoEditAndContinue",
        "NoRTTI",
        "WinMain"
    }
end
function commonBuildOptions()    
    -- some meta files can require bigobj
    buildoptions {
        "/permissive-",
        "/std:c++latest",
        "/bigobj",
        "/wd4275" -- non dll-interface struct used as base for dll-interface struct
    }
end
function commonIncludeDirs()
    -- common includes
    includedirs {
        SRC_ROOT, 
        SYTINNI_ROOT,
        SYTINNI_ROOT .. "/modules/",
        EXT_ROOT,
        DATA_ROOT
    }
end
function commonBuild()
    commonDefines()
    commonFlags()
    commonBuildOptions()
    commonIncludeDirs()
    commonLinks()
end

solution (SOL_NAME)
    -- cmake style configs
    location(PROJ_DIR)
    language "C++"
    platforms { "x32" }
    configurations { "Release", "RelWithDebInfo" }
    windowstargetplatformversion "10.0.19041.0"
    
    startproject "launcher"
    
    -- standard flags/defines/postbuild for different configs
    configuration "Release"
        debugdir (TARGET_DIR .. "Release")
        defines { "NDEBUG" }
        flags { "Optimize" }
        targetdir(TARGET_DIR .. "Release")
    configuration "RelWithDebInfo"
        debugdir (TARGET_DIR .. "RelWithDebInfo")
        defines { "NDEBUG" }
        flags { "Symbols", "Optimize" }
        targetdir(TARGET_DIR .. "RelWithDebInfo")
    configuration {}
        
    -- base editor module, could use the core editor for this but it seems more confusing for a user
    project "launcher"
        commonBuild()
        kind "WindowedApp"
        
        defines { "BUILDING_LAUNCHER" }
                
        includedirs {
            SYTINNI_ROOT .. "/core"
        }
        files {
            SYTINNI_ROOT .. "/launcher/**.h",
            SYTINNI_ROOT .. "/launcher/**.cpp",
            SYTINNI_ROOT .. "/launcher/**.rc"
        }
    
    project "utinni_core"
        commonBuild()
        kind "SharedLib"
        
        defines { "BUILDING_CORE" }
                
        libdirs { 
            EXT_ROOT .. "nvapi/x86",
        }
        links { "nvapi" }
        
        includedirs {
            SYTINNI_ROOT .. "/core",
            EXT_ROOT
        }
        files {
            SYTINNI_ROOT .. "/core/**.h",
            SYTINNI_ROOT .. "/core/**.cpp",
            SYTINNI_ROOT .. "/core/**.rc",
            EXT_ROOT .. "/ImGuizmo/**.h",
            EXT_ROOT .. "/ImGuizmo/**.cpp",
            EXT_ROOT .. "/DetourXS/**.h",
            EXT_ROOT .. "/DetourXS/**.cpp"
        }
        
        configuration "Release"
            postbuildcommands {
                "xcopy /Y /E /d \"" .. SOL_DATA_DIR .. "\" " .. "\"" .. (SOL_BUILD_DIR .. TARGET_DIR .. "Release") .. "\""
            }
        configuration "RelWithDebInfo"
            postbuildcommands {
                "xcopy /Y /E /d \"" .. SOL_DATA_DIR .. "\" " .. "\"" .. (SOL_BUILD_DIR .. TARGET_DIR .. "RelWithDebInfo") .. "\""
            }
        
function addPlugin(name)
    project (name)
        commonBuild()
        kind "SharedLib"
                
        links { "utinni_core", "d3dx9" }
        
        libdirs {
            EXT_ROOT .. "d3dx9/lib"
        }
        includedirs {
            SYTINNI_ROOT .. "/core",
            PLUGINS_ROOT .. "/" .. name,
            EXT_ROOT .. "d3dx9/include"
        }
        files {
            PLUGINS_ROOT .. "/" .. name .. "/**.h",
            PLUGINS_ROOT .. "/" .. name .. "/**.cpp",
            PLUGINS_ROOT .. "/" .. name .. "/**.rc"
        }
end

-- Add plugins
for dir in io.popen( [[dir "]] .. PLUGINS_ROOT .. [[\" /b /ad]]):lines() do addPlugin(dir) end

-- The default plugins added to new inis, in order of load
solution (SOL_NAME)
    defines { "DEFAULT_PLUGINS=\"sytners_toolbox,sytners_fx\"" }