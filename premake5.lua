IncludeDir = {}
IncludeDir["assimp"] = "%{wks.location}\\Dependencies\\assimp"

workspace "Assimp Compile Library"
  architecture "x64"
  startproject "Assimp Compile Application"
  
  configurations
  {
    "Debug",
    "Release"
  }
  
  flags
  {
	  "MultiProcessorCompile"
  }
  
  outputdir = "%{wks.location}/bin/%{cfg.buildcfg}"
  interdir = "%{wks.location}/bin_int"
  
  include "Assimp Compile Application"