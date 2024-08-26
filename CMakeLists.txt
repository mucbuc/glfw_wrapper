set(CMAKE_CXX_STANDARD 20)
set(DAWN_FETCH_DEPENDENCIES ON)
set(CMAKE_XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD)")
set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")

project(glfw_wrapper_lib)
add_subdirectory("lib/glfw" EXCLUDE_FROM_ALL)

add_library(glfw_wrapper STATIC src/glfw_wrapper.h)
target_sources(glfw_wrapper PRIVATE src/glfw_wrapper.cpp)

target_link_libraries(glfw_wrapper PUBLIC
    glfw
)
