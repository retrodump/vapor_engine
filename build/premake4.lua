-- vapor Build Settings (Premake)
-- Written by triton (2008-2009)

solution "vapor"
	configurations { "Debug", "Release" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	location ( _ACTION )
	objdir 	( _ACTION .. "/obj" )

	project "vaporEngine"
		kind "StaticLib"
		language "C++"

		location ( _ACTION )
		targetdir ( _ACTION .. "/lib" )

		-- Source files
		files {
			"../inc/Engine/**.h",
			"../src/Engine/**.h",
			"../src/Engine/**.cpp",
		}

		-- Include directories
		includedirs {
			"../inc",
			"../dep/boost_1_39_0",
			"../dep/fd_delegate/include",
			"../dep/freetype/include",
			"../dep/glew/include",
			"../dep/libogg-1.1.3/include",
			"../dep/libvorbis-1.2.0/include",
			"../dep/lua/include",
			"../dep/luabind/include",
			"../dep/OpenAL 1.1 SDK/include",
			"../dep/physfs-2.0.0/include",
			"../dep/SFML/include",
		}

		configuration "vs*"
			includedirs {
				"../dep/vld/include",
			}


		-- include only on Windows
		if os.is("windows") then
			includedirs { "../dep/vld/include" }
		end

		-- Pre-compiled headers
		pchheader "vapor/PCH.h"
		pchsource "../src/Engine/PCH.cpp"