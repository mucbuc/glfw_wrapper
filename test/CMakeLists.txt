cmake_minimum_required(VERSION 3.27)

project(Example)

add_executable(Example src/example.cpp)

set_target_properties(Example PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)

add_subdirectory(lib/glfw_wrapper)
add_subdirectory(lib/asserter)

target_link_libraries(Example PRIVATE 
  glfw_wrapper
)

target_include_directories(Example SYSTEM PUBLIC ${CMAKE_SOURCE_DIR})
