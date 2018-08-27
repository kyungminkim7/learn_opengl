cmake_minimum_required(VERSION 3.0)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
add_subdirectory(glad)

link_libraries(glad glfw)

add_executable(lesson1_window "src/lesson1_window.cpp")
add_executable(lesson1_exercise "src/lesson1_exercise.cpp")
