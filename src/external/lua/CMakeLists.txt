cmake_minimum_required (VERSION 3.3)

project(Lua C)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

file(GLOB_RECURSE SRC
    "src/*.c"
    "src/*.h"
)

add_library(lua ${SRC})