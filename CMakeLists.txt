set(CMAKE_CXX_STANDARD 20)
set(DAWN_FETCH_DEPENDENCIES ON)

project(glfw_wrapper_lib)

add_library(glfw_wrapper STATIC src/glfw_wrapper.h)
target_sources(glfw_wrapper PRIVATE src/glfw_wrapper.cpp)

target_link_libraries(glfw_wrapper PRIVATE
    glfw
)
