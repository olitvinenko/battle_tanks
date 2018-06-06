cmake_minimum_required (VERSION 3.3)
project(TankMain CXX)

include_directories(
	${FileSystem_SOURCE_DIR}
	${Platform_SOURCE_DIR}
)

link_directories(${Boost_LIBRARY_DIR})

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501) # Windows XP
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
	add_executable(tankmain WIN32 Main.cpp)
elseif(APPLE)
	add_executable(tankmain Main.cpp) # hack to enable retina display support

elseif(UNIX)
	add_executable(tankmain Main.cpp)
else()
	message(FATAL_ERROR "Unknown platform")
endif()

target_link_libraries(tankmain
	filesystem
)