cmake_minimum_required (VERSION 3.3)
project(tanks CXX)

link_directories(${Boost_LIBRARY_DIR})
	
############################ COMMON #####################################
include_directories(
	${Utf8cpp_SOURCE_DIR}	
	${GLFW_SOURCE_DIR}/include
	${Lua_SOURCE_DIR}/src
	${pluto_SOURCE_DIR}
	${box2d_SOURCE_DIR}
	${jsoncpp_SOURCE_DIR}
)
#########################################################################

############################ PLATFORM ###################################

include_directories(
	Platform
)

file(GLOB_RECURSE SRC_PLATFORM
    "Platform/*.cpp"
    "Platform/*.h"
)

source_group("platform" FILES ${SRC_PLATFORM})

#########################################################################

############################ FILE SYSTEM ################################
include_directories(
	FileSystem
)

set(SRC_FILE_SYSTEM
	FileSystem/FileSystem.cpp
	FileSystem/FileSystem.h
)

if(WIN32)
	list(APPEND SRC_FILE_SYSTEM FileSystem/FileSystemWin32.cpp FileSystem/FileSystemWin32.h)
elseif(UNIX)
	list(APPEND SRC_FILE_SYSTEM FileSystem/FileSystemUnix.cpp FileSystem/FileSystemUnix.h)
endif()

source_group("filesystem" FILES ${SRC_FILE_SYSTEM})
#########################################################################

############################ RENDER #####################################
include_directories(
	Render
)

set(SRC_RENDER
	#Render/RenderBase.h
	#
	#Render/DrawingContext.h
	#Render/DrawingContext.cpp
	#
	#Render/TGAImage.h
	#Render/TGAImage.cpp
	#
	#Render/TextureManager.h	
	#Render/TextureManager.cpp
	#
	#Render/RenderOpenGL.h
)

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501) # Windows XP
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
	
	#list(APPEND SRC_RENDER Render/RenderOpenGL.cpp)
elseif(IOS)
	list(APPEND SRC_RENDER	
		Render/GLESProgram.cpp
		Render/GLESProgram.h
		
		Render/RenderGLES2.cpp
	)
elseif(UNIX)
	#list(APPEND SRC_RENDER Render/RenderOpenGL.cpp)
endif()

source_group("render" FILES ${SRC_RENDER})
#########################################################################

############################ SHELL ######################################
include_directories(
	shell
)

file(GLOB_RECURSE SRC_SHELL
    "Shell/*.cpp"
    "Shell/*.h"
)

source_group("shell" FILES ${SRC_SHELL})
#########################################################################

############################ UI #########################################
include_directories(
	UI
)

file(GLOB_RECURSE SRC_UI
    "UI/*.cpp"
    "UI/*.h"
)

source_group("ui" FILES ${SRC_UI})
#########################################################################

############################ AI #########################################
include_directories(
	ai
)

file(GLOB_RECURSE SRC_AI
    "ai/*.cpp"
    "ai/*.h"
)

source_group("ai" FILES ${SRC_AI})
#########################################################################

############################ APP #########################################
include_directories(
	app
)

file(GLOB_RECURSE SRC_APP
    "app/*.cpp"
    "app/*.h"
)

source_group("app" FILES ${SRC_APP})
#########################################################################

############################ AS #########################################
include_directories(
    as
)

file(GLOB_RECURSE SRC_AS
    "as/*.cpp"
    "as/*.h"
)

source_group("as" FILES ${SRC_AS})
#########################################################################

############################ MATH UTILS ################################
include_directories(
    Math
)

file(GLOB_RECURSE SRC_MATH
    "Math/*.cpp"
    "Math/*.h"
)

source_group("mathutils" FILES ${SRC_MATH})
#########################################################################

############################ CONFIG ################################
include_directories(
    config
)

file(GLOB_RECURSE SRC_CONFIG
    "config/*.cpp"
    "config/*.h"
)

source_group("config" FILES ${SRC_CONFIG})
#########################################################################

############################ CTX ################################
include_directories(
    ctx
)

file(GLOB_RECURSE SRC_CTX
    "ctx/*.cpp"
    "ctx/*.h"
)

source_group("ctx" FILES ${SRC_CTX})
#########################################################################

############################ gc ################################
include_directories(
    gc
)

file(GLOB_RECURSE SRC_GC
    "gc/*.cpp"
    "gc/*.h"
)

source_group("gc" FILES ${SRC_GC})
#########################################################################

############################ gv ################################
include_directories(
    gv
)

file(GLOB_RECURSE SRC_GV
    "gv/*.cpp"
    "gv/*.h"
)

source_group("gv" FILES ${SRC_GV})
#########################################################################

############################ loc ################################
include_directories(
    loc
)

file(GLOB_RECURSE SRC_LOC
    "loc/*.cpp"
    "loc/*.h"
)

source_group("loc" FILES ${SRC_LOC})
#########################################################################

############################ LUA ETC ################################
include_directories(
    luaetc
)

file(GLOB_RECURSE SRC_LUAETC
    "luaetc/*.cpp"
    "luaetc/*.h"
)

source_group("luaetc" FILES ${SRC_LUAETC})
#########################################################################

############################ MF ################################
include_directories(
    mf
)

file(GLOB_RECURSE SRC_MP
    "mf/*.cpp"
    "mf/*.h"
)

source_group("mf" FILES ${SRC_MP})
#########################################################################

############################ PLATGLFW ################################
include_directories(
    platglfw
)

file(GLOB_RECURSE SRC_PLATGLFW
    "platglfw/*.cpp"
    "platglfw/*.h"
)

source_group("platglfw" FILES ${SRC_PLATGLFW})
#########################################################################

############################ RENDER ################################
include_directories(
    render
)

file(GLOB_RECURSE SRC_RENDER
    "render/*.cpp"
    "render/*.h"
)

source_group("render" FILES ${SRC_RENDER})
#########################################################################

############################ SCRIPT ################################
include_directories(
    script
)

file(GLOB_RECURSE SRC_SCRIPT
    "script/*.cpp"
    "script/*.h"
)

source_group("script" FILES ${SRC_SCRIPT})
#########################################################################

############################ VIDEO ################################
include_directories(
    video
)

file(GLOB_RECURSE SRC_VIDEO
    "video/*.cpp"
    "video/*.h"
)

source_group("video" FILES ${SRC_VIDEO})
#########################################################################

############################ GCLUA ################################
include_directories(
    gclua
)

file(GLOB_RECURSE SRC_GCLUA
    "gclua/*.cpp"
    "gclua/*.h"
)

source_group("gclua" FILES ${SRC_GCLUA})

############################ GLFW #######################################
include_directories(
	#GLFW
)

file(GLOB_RECURSE SRC_GLFW
   # "GLFW/*.cpp"
   # "GLFW/*.h"
)

#source_group("glfw" FILES ${SRC_GLFW})
#########################################################################

############################ PATHFINDING ################################
include_directories(
	#Pathfinding
)

file(GLOB_RECURSE SRC_PATHFINDING
    "Pathfinding/*.cpp"
    "Pathfinding/*.h"
)

source_group("pathfinding" FILES ${SRC_PATHFINDING})
#########################################################################

############################ ENGINE #######################################
include_directories(
	#Engine
)

file(GLOB_RECURSE SRC_ENGINE
    "Engine/*.cpp"
    "Engine/*.h"
)

if (WIN32)
	foreach(FILE ${SRC_ENGINE}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
		

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")

		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endif()

#########################################################################


############################ GAME #######################################
include_directories(
	#Game
)

file(GLOB_RECURSE SRC_GAME
    "Game/*.cpp"
    "Game/*.h"
)

if (WIN32)
	foreach(FILE ${SRC_GAME}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
		
		include_directories(${PARENT_DIR})		

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")

		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endif()

#########################################################################


set(tanks_SOURCES
	${SRC_AI}
    ${SRC_APP}
    ${SRC_AS}
	${SRC_MATH}
    ${SRC_CONFIG}
    ${SRC_CTX}
    ${SRC_GC}
    ${SRC_GV}
    ${SRC_LOC}
    ${SRC_LUAETC}
    ${SRC_MP}
    ${SRC_PLATGLFW}
    ${SRC_RENDER}
    ${SRC_SCRIPT}
    ${SRC_SHELL}
    ${SRC_VIDEO}
    ${SRC_GCLUA}
    ${SRC_UI}

    ${SRC_FILE_SYSTEM}
    
	#${SRC_PLATFORM}
	#${SRC_GLFW}
	#${SRC_ENGINE}
	#${SRC_PATHFINDING}
)

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
	
	list(APPEND tanks_SOURCES Main.cpp)
	
	add_executable(tanks ${tanks_SOURCES})
elseif(IOS)

elseif(UNIX)
	list(APPEND tanks_SOURCES Main.cpp)
	
	add_executable(tanks ${tanks_SOURCES})
else()
	message(FATAL_ERROR "Unknown platform")
endif()

if(WIN32)
	target_link_libraries(tanks
		opengl32
		glfw
		lua
		box2d
		jsoncpp
		pluto
		${GLFW_LIBRARIES}
	)
elseif(UNIX)
	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIR})

	target_link_libraries(tanks
		glfw
		lua
		box2d
		jsoncpp
		pluto
		${GLFW_LIBRARIES}
		${OPENGL_LIBRARIES}
	)
endif()
