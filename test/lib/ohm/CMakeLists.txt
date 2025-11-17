cmake_minimum_required(VERSION 3.27)
project(ohm)

add_library(ohm INTERFACE)

target_include_directories(ohm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(ohm PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)


