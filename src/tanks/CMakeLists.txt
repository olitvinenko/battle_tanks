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
	${engine_SOURCE_DIR}
)
#########################################################################


############################ ENGINE #######################################
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB_RECURSE SRC_ALL
	"*.h*"
	"*.c*"
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


set(tanks_SOURCES ${SRC_ALL})

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
		engine
	)
elseif(UNIX)
	include_directories(
		${audio_SOURCE_DIR}
	)

	target_link_libraries(tanks
		engine
	)
endif()
