cmake_minimum_required (VERSION 3.3)

project(Lua C)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DLUA_COMPAT_MODULE) 

file(GLOB_RECURSE SRC
    "src/*.c"
    "src/*.h"
)

get_filename_component(LUA_MAIN_C ${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c ABSOLUTE)

list(REMOVE_ITEM SRC "${LUA_MAIN_C}")

add_library(lua ${SRC})