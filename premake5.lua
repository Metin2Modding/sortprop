workspace "sortprop"
  -- Common settings
  location "build"
  cppdialect "C++latest"
  architecture "x86_64"
  staticruntime "On"

  flags {
    "MultiProcessorCompile"
  } configurations {
    "Debug",
    "Release"
  }

  -- Project settings
  project "sortprop"
    kind "ConsoleApp"

    files {
      "src/*.cpp",
      "src/*.hpp"
    } includedirs {
      "vendor/fast_io/include",
      "vendor/xxHash"
    }

    -- Filter settings
    filter "configurations:Debug"
      symbols "On"

    filter "configurations:Release"
      optimize "On"
	  linktimeoptimization "On"
