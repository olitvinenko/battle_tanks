cmake_minimum_required (VERSION 3.4)

if(NOT IOS)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
	set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
	set(GLFW_USE_DWM_SWAP_INTERVAL ON CACHE BOOL "Set swap interval even when DWM compositing is enabled")
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
	
	add_subdirectory(glfw)
endif()

add_subdirectory(lua)
add_subdirectory(utfcpp)
add_subdirectory(jsoncpp)
add_subdirectory(box2d)