cmake_minimum_required(VERSION 3.0)
project(gl_lessons)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

link_libraries(
    glad
    glfw
    gl_util
    ${BOOST_LIBRARIES}
    stb
    glm
)

add_executable(lesson1_window "src/lesson1_window.cpp")
add_executable(lesson1_exercise "src/lesson1_exercise.cpp")
add_executable(lesson2_shaders "src/lesson2_shaders.cpp")
add_executable(lesson3_textures "src/lesson3_textures.cpp")
add_executable(lesson4_transformations "src/lesson4_transformations.cpp")
add_executable(lesson5_coordinate_systems "src/lesson5_coordinate_systems.cpp")
