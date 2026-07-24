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

  target_link_libraries(glfw_wrapper PRIVATE
     glfw
  )

  target_include_directories(glfw_wrapper SYSTEM PUBLIC       
      ${CMAKE_CURRENT_LIST_DIR}/lib/glfw/include
  )

  if (NOT TARGET glfw)
    add_subdirectory(lib/glfw)
  endif()
else()
  target_compile_options(glfw_wrapper PRIVATE --use-port=contrib.glfw3)
endif()

target_include_directories(glfw_wrapper SYSTEM PUBLIC       
    ${CMAKE_SOURCE_DIR}/lib
)
