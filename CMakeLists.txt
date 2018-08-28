cmake_minimum_required(VERSION 3.0)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(glad)

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} glad glfw)

add_library(gl_util "src/shader_program.cpp")
target_include_directories(gl_util
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

add_executable(lesson1_window "src/lesson1_window.cpp")
add_executable(lesson1_exercise "src/lesson1_exercise.cpp")
