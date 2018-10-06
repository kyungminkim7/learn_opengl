cmake_minimum_required(VERSION 3.0)
project(gl_lessons)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

link_libraries(
    glad
    glfw
    gl_util
    stb
    glm
)

add_executable(lesson1_window "src/lesson1_window.cpp")
add_executable(lesson1_exercise "src/lesson1_exercise.cpp")
add_executable(lesson2_shaders "src/lesson2_shaders.cpp")
add_executable(lesson3_textures "src/lesson3_textures.cpp")
add_executable(lesson4_transformations "src/lesson4_transformations.cpp")
add_executable(lesson5_coordinate_systems "src/lesson5_coordinate_systems.cpp")
add_executable(lesson6_camera "src/lesson6_camera.cpp")
add_executable(lesson7_colors "src/lesson7_colors.cpp")
add_executable(lesson8_basic_lighting "src/lesson8_basic_lighting.cpp")
