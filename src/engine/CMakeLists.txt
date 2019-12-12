cmake_minimum_required (VERSION 3.3)

project(engine)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(
	${Utf8cpp_SOURCE_DIR}	
	${GLFW_SOURCE_DIR}/include
	${Lua_SOURCE_DIR}/src
	${pluto_SOURCE_DIR}
	${box2d_SOURCE_DIR}
	${jsoncpp_SOURCE_DIR}
	${audio_SOURCE_DIR}
)

############################ FILE SYSTEM ################################
set(SRC_FILE_SYSTEM
	filesystem/FileSystem.cpp
	filesystem/FileSystem.h
)

if(WIN32)
	list(APPEND SRC_FILE_SYSTEM filesystem/FileSystemWin32.cpp filesystem/FileSystemWin32.h)
elseif(UNIX)
	list(APPEND SRC_FILE_SYSTEM filesystem/FileSystemUnix.cpp filesystem/FileSystemUnix.h)
endif()

foreach(FILE ${SRC_FILE_SYSTEM})
	get_filename_component(MY_RESULT_ABSOLUTE_PATH_VAR "${CMAKE_CURRENT_LIST_DIR}/${FILE}" ABSOLUTE)
	list(APPEND SRC_FILE_SYSTEM_ABSOLUTE ${MY_RESULT_ABSOLUTE_PATH_VAR})
endforeach()

#########################################################################


file(GLOB_RECURSE SRC_ALL
	"*.h*"
	"*.c*"
)

file(GLOB_RECURSE SRC_FILE_SYSTEM_ALL
	"filesystem/*.h*"
	"filesystem/*.c*"
)

list(REMOVE_ITEM SRC_ALL ${SRC_FILE_SYSTEM_ALL})
list(APPEND SRC_ALL ${SRC_FILE_SYSTEM_ABSOLUTE})

foreach(FILE ${SRC_ALL})
	# Get the directory of the source file
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

	# Remove common directory prefix to make the group
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

	# Make sure we are using windows slashes
	string(REPLACE "/" "\\" GROUP "${GROUP}")

	include_directories(${PARENT_DIR})

	source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_library(engine ${SRC_ALL})

if(WIN32)
	target_link_libraries(engine
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

	target_link_libraries(engine
		glfw
		lua
		box2d
		jsoncpp
		pluto
		audio
		${GLFW_LIBRARIES}
		${OPENGL_LIBRARIES}
	)
endif()

