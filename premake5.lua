AssimpInclude = "%{prj.location}\\Dependencies\\assimp" 

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
  }

  externalincludedirs
  {
	  "%{AssimpInclude}\\include"
  }
  
  includedirs
  {
    "%{prj.location}/src",
  }
  
  externalwarnings "Off"
  
  libdirs
  {
    "%{AssimpInclude}/lib/Debug",
    "%{AssimpInclude}/lib/Release"
  }

  flags
  {
  	"MultiProcessorCompile"
  }

  filter "configurations:Debug"
    postbuildcommands 
    {
      "xcopy  /r /y /q \"%{AssimpInclude}\\bin\\Debug\\assimp-vc142-mtd.dll\" \"$(OutDir)\""
    }
    
  filter "configurations:Release"
    postbuildcommands 
    {
      "xcopy  /r /y /q \"%{AssimpInclude}\\bin\\Release\\assimp-vc142-mt.dll\" \"$(OutDir)\""
    }

  filter "configurations:Publish"
    postbuildcommands 
    {
      "xcopy  /r /y /q \"%{AssimpInclude}\\bin\\Release\\assimp-vc142-mt.dll\" \"$(OutDir)\""
    }

  warnings 'Extra'
  
  filter "configurations:Debug"
    symbols "On"
    defines {"_DEBUG"}
    links{"assimp-vc142-mtd.lib"}
  
  filter "configurations:Release"
    optimize "On"
    defines{"_RELEASE"}
    links{"assimp-vc142-mt.lib"}
  
  filter "configurations:Publish"
    optimize "On"
    defines{"_RELEASE, _PUBLISH"}
    links{"assimp-vc142-mt.lib"}