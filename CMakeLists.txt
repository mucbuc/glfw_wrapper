cmake_minimum_required(VERSION 3.27)

project(glfw_wrapper)

add_library(glfw_wrapper STATIC src/glfw_wrapper.hpp)

set_target_properties(glfw_wrapper PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

target_sources(glfw_wrapper PRIVATE 
    src/glfw_wrapper.cpp)

if (NOT EMSCRIPTEN)
add_subdirectory(lib/glfw)

target_link_libraries(glfw_wrapper PRIVATE
    glfw
)

target_include_directories(glfw_wrapper SYSTEM PUBLIC       
    ${CMAKE_CURRENT_LIST_DIR}/lib/glfw/include
)
endif()

target_include_directories(glfw_wrapper SYSTEM PUBLIC       
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}
)
