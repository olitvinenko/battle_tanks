cmake_minimum_required (VERSION 3.4)

project(jsoncpp CXX)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SRCS
    "*.c*"
    "*.h*"
)

add_library(jsoncpp ${SRCS})
