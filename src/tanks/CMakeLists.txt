cmake_minimum_required (VERSION 3.3)
project(tanks CXX)

link_directories(${Boost_LIBRARY_DIR})
	
############################ COMMON #####################################
include_directories(
	${Utf8cpp_SOURCE_DIR}	
	${GLFW_SOURCE_DIR}/include
	${Lua_SOURCE_DIR}/src	
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

############################ APPLICATION ################################
include_directories(
	Application
)

file(GLOB_RECURSE SRC_APPLICATION
    "Application/*.cpp"
    "Application/*.h"
)

source_group("application" FILES ${SRC_APPLICATION})
#########################################################################

############################ SHELL ######################################
include_directories(
	Shell
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

############################ GUI #########################################
include_directories(
	GUI
)

file(GLOB_RECURSE SRC_GUI
    "GUI/*.cpp"
    "GUI/*.h"
)

source_group("gui" FILES ${SRC_GUI})
#########################################################################

############################ GLFW #######################################
include_directories(
	GLFW
)

file(GLOB_RECURSE SRC_GLFW
    "GLFW/*.cpp"
    "GLFW/*.h"
)

source_group("glfw" FILES ${SRC_GLFW})
#########################################################################

############################ PATHFINDING ################################
include_directories(
	Pathfinding
)

file(GLOB_RECURSE SRC_PATHFINDING
    "Pathfinding/*.cpp"
    "Pathfinding/*.h"
)

source_group("pathfinding" FILES ${SRC_PATHFINDING})
#########################################################################

############################ ENGINE #######################################
include_directories(
	Engine
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
	Game
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
	${SRC_MATH}
	${SRC_PLATFORM}
	${SRC_FILE_SYSTEM}
	${SRC_RENDER}
	${SRC_UI}
	${SRC_GUI}
	${SRC_GLFW}
	${SRC_ENGINE}
	${SRC_GAME}
	${SRC_APPLICATION}
	${SRC_SHELL}
	${SRC_PATHFINDING}
)

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501) # Windows XP
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
		${GLFW_LIBRARIES}
		${OPENGL_LIBRARIES}
	)
endif()