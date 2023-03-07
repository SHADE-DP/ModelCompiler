outputdir = "%{wks.location}/bin/%{cfg.buildcfg}"
interdir = "%{wks.location}/bin_int"
workspace "ModelCompile"
architecture "x64"
  configurations
  {
    "Release",
    "Debug"
  }

project "ModelCompiler"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
  targetdir (outputdir)
  objdir    (interdir)
  systemversion "latest"

  files  
  {
    "%{prj.location}/src/**.h",
    "%{prj.location}/src/**.cpp",
    "%{prj.location}/src/**.hpp"
  }
  
  includedirs
  {
    "%{prj.location}/src"
  }
  
  externalwarnings "Off"

  flags
  {
  	"MultiProcessorCompile"
  }

  warnings 'Extra'

  defines {
    "TINYGLTF_IMPLEMENTATION",
    "TINYGLTF_NO_EXTERNAL_IMAGE",
    "TINYGLTF_NO_INCLUDE_STB_IMAGE",
    "TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE",
    "TINYGLTF_NO_STB_IMAGE_WRITE",
    "TINYGLTF_NO_STB_IMAGE"
  }
  
  filter "configurations:Debug"
    symbols "On"
    defines {"_DEBUG"}
  
  filter "configurations:Release"
    optimize "On"
    defines{"_RELEASE"}
  
  filter "configurations:Publish"
    optimize "On"
    defines{"_RELEASE, _PUBLISH"}