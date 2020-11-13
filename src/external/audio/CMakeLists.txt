cmake_minimum_required (VERSION 3.3)

project(audio)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

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

	include_directories(${PARENT_DIR})

	source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_library(audio ${SRC_ALL})

if(WIN32)
	message ("audio lib is not implemented")
elseif(UNIX)

	find_package(OpenAL REQUIRED)

	if (OPENAL_FOUND)
		include_directories(${OPENAL_INCLUDE_DIR})

		target_link_libraries(audio
			${OPENAL_LIBRARY}
		)
	else()
		message("can't find OpenAL library")
	endif()

	
endif()
