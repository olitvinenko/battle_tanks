cmake_minimum_required (VERSION 3.3)

project(Lua)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DLUA_COMPAT_MODULE) 

file(GLOB_RECURSE SRC
    "src/*.c"
    "src/*.h"
)

list (REMOVE_ITEM SRC src/loslib.c)

add_library(lua ${SRC})
