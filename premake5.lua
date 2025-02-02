workspace "sortprop"
  -- Common settings
  location "build"
  cppdialect "C++latest"
  architecture "x86_64"
  staticruntime "On"

  flags {
    "MultiProcessorCompile"
  } buildoptions {
    "/utf-8"
  } configurations {
    "Debug",
    "Release"
  }

  -- Project settings
  project "sortprop"
    kind "ConsoleApp"

    files {
      "src/*.cpp",
      "src/*.h",

      "vendor/xxHash/xxhash.c",
      "vendor/xxHash/xxhash.h"
    } includedirs {
      "vendor/fast_io/include",
      "vendor/spdlog/include",
      "vendor/xxHash"
    }

    -- Filter settings
    filter "configurations:Debug"
      symbols "On"

    filter "configurations:Release"
      optimize "On"
      linktimeoptimization "On"
