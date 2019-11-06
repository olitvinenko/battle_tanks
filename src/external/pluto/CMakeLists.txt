cmake_minimum_required (VERSION 3.3)

project(pluto)

include_directories(${Lua_SOURCE_DIR}/src)

add_library(pluto
	pluto.c
)
