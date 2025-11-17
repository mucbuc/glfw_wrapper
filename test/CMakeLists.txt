cmake_minimum_required(VERSION 3.27)

project(Example)

enable_testing()

add_executable(Example src/example.cpp)

set_target_properties(Example PROPERTIES
  CXX_STANDARD 20 
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  COMPILE_WARNING_AS_ERROR ON
)

add_subdirectory(lib/glfw_wrapper)
add_subdirectory(lib/asserter)
add_subdirectory(lib/dynamo)
add_subdirectory(lib/circuit)

target_link_libraries(Example PRIVATE 
  glfw_wrapper
  dynamo
  circuit
)

target_include_directories(Example SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_test(
  NAME basic_test
  COMMAND Example
)
