cmake_minimum_required (VERSION 3.3)
project(tanks CXX)
	
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


############################ ENGINE #######################################
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB_RECURSE SRC_ALL
    "math/*.cpp"
    "math/*.h"
    
    "ai/*.cpp"
    "ai/*.h"
    
    "app/*.cpp"
    "app/*.h"
    
    "as/*.cpp"
    "as/*.h"
    
    "config/*.cpp"
    "config/*.h"
    
    "ctx/*.cpp"
    "ctx/*.h"
    
    "gc/*.cpp"
    "gc/*.h"
    
    "gclua/*.cpp"
    "gclua/*.h"
    
    "gv/*.cpp"
    "gv/*.h"
    
    "loc/*.h"
    "loc/*.cpp"
    
    "luaetc/*.cpp"
    "luaetc/*.h"
    
    "memory/*.cpp"
    "memory/*.h"
    
    "mf/*.cpp"
    "mf/*.h"
    
    "platglfw/*.cpp"
    "platglfw/*.h"
    
    "render/*.cpp"
    "render/*.h"
    
    "script/*.cpp"
    "script/*.h"
    
    "shell/*.cpp"
    "shell/*.h"
    
    "ui/*.cpp"
    "ui/*.h"
    
    "video/*.cpp"
    "video/*.h"
)

foreach(FILE ${SRC_ALL})
	# Get the directory of the source file
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
		
	# Remove common directory prefix to make the group
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

	# Make sure we are using windows slashes
	string(REPLACE "/" "\\" GROUP "${GROUP}")

	source_group("${GROUP}" FILES "${FILE}")
endforeach()

#########################################################################


set(tanks_SOURCES
	${SRC_ALL}
	${SRC_FILE_SYSTEM}
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
