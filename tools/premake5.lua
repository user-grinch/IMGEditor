workspace "IMGEditor"
    configurations { 
        "Debug", 
        "Release" 
    }

    platforms {
        "Win64"
    }

    language "C++"
    cppdialect "C++20"
    staticruntime "On"
    characterset "ASCII"
    location "../build"
    targetdir "../build/bin"
    kind "WindowedApp"
    
    includedirs {
        "../include/"
    }

project "depend"
    kind "StaticLib"

    files { 
        "../include/**"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "IMGEditorApp"
    architecture "x64"
    links { 
        "d3d9",
        "depend"
    }

    files { 
        "../src/**"
    }

    pchheader "pch.h"
    pchsource "../src/pch.cpp"

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        defines {
            "NDEBUG"
        }