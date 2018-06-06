cmake_minimum_required (VERSION 3.4)

project(FileSystem)

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(
	${Utf8cpp_SOURCE_DIR}
	${Platform_SOURCE_DIR}
)

set(FileSystem_SOURCES FileSystem.cpp FileSystem.h)

if(WIN32)
	list(APPEND FileSystem_SOURCES FileSystemWin32.cpp FileSystemWin32.h)
else()
	message("NOT WINDOWS")
endif()
   
add_library(filesystem ${FileSystem_SOURCES})