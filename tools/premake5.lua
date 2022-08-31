workspace "IMGEditor"
    configurations { 
        "Debug", 
        "Release" 
    }

    platforms {
        "Win64"
    }
    language "C++"
    cppdialect "C++latest"
    staticruntime "On"
    characterset "ASCII"
    location "../build"
    targetdir "../build/bin"
    kind "ConsoleApp"

    files { 
        "../include/**", 
        "../src/**"
    }
    
    includedirs {
        "../include/"
    }

project "IMGEditorApp"
    architecture "x64"
    links { 
        "d3d9"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        defines {
            "NDEBUG"
        }