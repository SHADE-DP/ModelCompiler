IncludeDir = {}
IncludeDir["assimp"] = "%{wks.location}\\Dependencies\\assimp"

workspace "ModelCompileLibrary"
  architecture "x64"
  startproject "Assimp Compile Application"
  
  configurations
  {
    "Debug",
    "Release",
    "Publish"
  }
  
  flags
  {
	  "MultiProcessorCompile"
  }
  
  outputdir = "%{wks.location}/bin/%{cfg.buildcfg}"
  interdir = "%{wks.location}/bin_int"
  
  include "ModelCompileLibrary"