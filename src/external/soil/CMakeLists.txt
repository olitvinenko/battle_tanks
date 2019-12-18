cmake_minimum_required (VERSION 3.4)

project(soil CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SRCS
    "src/SOIL*"
    "src/image*"
    "src/stb*"
)

foreach(FILE ${SRCS})
	get_filename_component(FILE "${FILE}" ABSOLUTE)
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "" GROUP "${PARENT_DIR}")

	string(REPLACE "/" "\\" GROUP "${GROUP}")

	source_group("${GROUP}" FILES "${FILE}")
endforeach()


add_library(soil ${SRCS})
