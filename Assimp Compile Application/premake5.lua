project "Assimp Compile Library"
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
	  "%{IncludeDir.assimp}\\include"
  }
  
  includedirs
  {
    "%{prj.location}/src",
  }
  
  externalwarnings "Off"
  
  libdirs
  {
    "%{IncludeDir.assimp}/lib/Debug",
    "%{IncludeDir.assimp}/lib/Release"
  }

  flags
  {
  	"MultiProcessorCompile"
  }

  filter "configurations:Debug"
    postbuildcommands 
    {
      "xcopy  /r /y /q \"%{IncludeDir.assimp}\\bin\\Debug\\assimp-vc142-mtd.dll\" \"$(OutDir)\""
    }
    
  filter "configurations:Release"
    postbuildcommands 
    {
      "xcopy  /r /y /q \"%{IncludeDir.assimp}\\bin\\Release\\assimp-vc142-mt.dll\" \"$(OutDir)\""
    }

  warnings 'Extra'
  
  filter "configurations:Debug"
    symbols "On"
    defines {"_DEBUG", "SHEDITOR"}
    links{"assimp-vc142-mtd.lib"}
  
  filter "configurations:Release"
    optimize "On"
    defines{"_RELEASE", "SHEDITOR"}
    links{"assimp-vc142-mt.lib"}