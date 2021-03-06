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
add_executable(lesson9_materials "src/lesson9_materials.cpp")
add_executable(lesson10_lighting_maps "src/lesson10_lighting_maps.cpp")
add_executable(lesson11_light_casters "src/lesson11_light_casters.cpp")
add_executable(lesson12_multiple_lights "src/lesson12_multiple_lights.cpp")
add_executable(lesson13_model "src/lesson13_model.cpp")
add_executable(lesson14_depth_testing "src/lesson14_depth_testing.cpp")
add_executable(lesson15_stencil_testing "src/lesson15_stencil_testing.cpp")
add_executable(lesson16_blending "src/lesson16_blending.cpp")
add_executable(lesson17_face_culling "src/lesson17_face_culling.cpp")
add_executable(lesson18_framebuffers "src/lesson18_framebuffers.cpp")
add_executable(lesson19_cubemaps "src/lesson19_cubemaps.cpp")
add_executable(lesson20_advanced_glsl "src/lesson20_advanced_glsl.cpp")
add_executable(lesson21_geometry_shaders "src/lesson21_geometry_shaders.cpp")
add_executable(lesson22_instancing "src/lesson22_instancing.cpp")
add_executable(lesson23_antialiasing "src/lesson23_antialiasing.cpp")
add_executable(lesson24_advanced_lighting "src/lesson24_advanced_lighting.cpp")
add_executable(lesson25_shadow_mapping "src/lesson25_shadow_mapping.cpp")
add_executable(lesson26_point_shadows "src/lesson26_point_shadows.cpp")
add_executable(lesson27_normal_mapping "src/lesson27_normal_mapping.cpp")
add_executable(lesson28_parallax_mapping "src/lesson28_parallax_mapping.cpp")
