cmake_minimum_required (VERSION 3.3)
project(tanks CXX)

link_directories(${Boost_LIBRARY_DIR})
	
############################ COMMON #####################################
include_directories(
	${Utf8cpp_SOURCE_DIR}	
	${GLFW_SOURCE_DIR}/include
	${Lua_SOURCE_DIR}/src
	
	Platform
)

set(SRC_COMMON
	Platform/Platform.h
)

source_group("common" FILES ${SRC_COMMON})
#########################################################################

############################ MATH UTILS ################################
include_directories(
	Math
)

set(SRC_MATH
	Math/Vector2.h
	Math/Rect.h
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

endif()

source_group("filesystem" FILES ${SRC_FILE_SYSTEM})
#########################################################################

############################ RENDER #####################################
include_directories(
	Render
)

set(SRC_RENDER
	Render/RenderBase.h
	
	Render/DrawingContext.h
	Render/DrawingContext.cpp
	
	Render/TGAImage.h
	Render/TGAImage.cpp
	
	Render/TextureManager.h	
	Render/TextureManager.cpp
	
	Render/RenderOpenGL.h
)

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501) # Windows XP
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
	
	list(APPEND SRC_RENDER Render/RenderOpenGL.cpp)
elseif(IOS)
	list(APPEND SRC_RENDER	
		Render/GLESProgram.cpp
		Render/GLESProgram.h
		
		Render/RenderGLES2.cpp
	)
elseif(UNIX)
	list(APPEND SRC_RENDER Render/RenderOpenGL.cpp)
endif()

source_group("render" FILES ${SRC_RENDER})
#########################################################################

set(tanks_SOURCES
	${SRC_MATH}
	${SRC_COMMON}
	${SRC_FILE_SYSTEM}
	${SRC_RENDER}
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

target_link_libraries(tanks
	opengl32
	glfw
	lua
	${GLFW_LIBRARIES}
)