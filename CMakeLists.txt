cmake_minimum_required(VERSION 3.0)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
add_subdirectory(glad)

link_libraries(glad glfw)

add_executable(hello_window "src/hello_window.cpp")
target_link_libraries(hello_window)
