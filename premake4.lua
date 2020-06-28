#!/usr/bin/lua

solution "NC Editor"
project "nc_editor"
	language "C"
	kind "ConsoleApp"
	location "src"
	targetdir "bin"
	objdir "obj"
	links { "ncurses" }
	includedirs { "src/ncurses-screen/src" }
	files {
		"src/main.c",
		"src/buffer.c",
		"src/buffer.h",
		"src/cmdlib.c",
		"src/cmdlib.h",
		"src/editor.c",
		"src/editor.h",
		"src/log.c",
		"src/log.h",
		"src/ncurses-screen/src/screen.c",
		"src/ncurses-screen/src/screen.h",
	}

	configurations { "Debug", "Release" }
	configuration "Debug"
		buildoptions { "-O0", "-g", "-Wall", "-D_GNU_SOURCE=1" }
		targetname "debug"

	configuration "Release"
		buildoptions { "-O3", "-Wall", "-D_GNU_SOURCE=1" }
		targetname "release"

