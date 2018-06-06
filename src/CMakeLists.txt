cmake_minimum_required (VERSION 3.4)

project(battle_tanks)

if(APPLE OR UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

if(WIN32)
	#TODO: message()
else()
	#TODO: message()
endif()

# external libs
add_subdirectory(external)

# libs
add_subdirectory(FileSystem)
add_subdirectory(Platform)

# top level lib - the only one the application links with
add_subdirectory(tankmain)
