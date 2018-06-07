cmake_minimum_required (VERSION 3.4)

project(battle_tanks)

if(APPLE OR UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_subdirectory(external)
add_subdirectory(tanks)

#copy to the main project folder
set(source_dir ../data)
set(destination_dir tanks)

file( COPY ${source_dir}
      DESTINATION ${destination_dir}
)