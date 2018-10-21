cmake_minimum_required (VERSION 3.4)

project(box2d CXX)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SRCS
    "*.c*"
    "*.h*"
)

if (WIN32)
	foreach(FILE ${SRCS}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/Box2D" "" GROUP "${PARENT_DIR}")

		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")

		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endif()

add_library(box2d ${SRCS})