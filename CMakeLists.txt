cmake_minimum_required(VERSION 3.5...3.10)
project(learn_opengl VERSION 1.0.0)

add_subdirectory(extern)

add_library(lgl
    src/Camera.cpp
    src/Frame.cpp
    src/Shader.cpp
    src/ShaderProgram.cpp
    src/Texture2D.cpp
)
add_library(lgl::lgl ALIAS lgl)

target_include_directories(lgl
   PUBLIC
       "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"    
       "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"    
       "$<INSTALL_INTERFACE:include>"
   PRIVATE
       "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(lgl PUBLIC
    glad::glad
    glfw
    glm::glm
    stb::stb
    ${CMAKE_DL_LIBS}
)

target_compile_features(lgl PUBLIC cxx_std_14)

file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(l1_hello_window)
add_subdirectory(l2_hello_triangle)
add_subdirectory(l3_shaders)
add_subdirectory(l4_textures)
add_subdirectory(l5_transformations)
add_subdirectory(l6_coordinate_systems)
add_subdirectory(l7_camera)
add_subdirectory(l8_colors)
add_subdirectory(l9_basic_lighting)
add_subdirectory(l10_materials)
add_subdirectory(l11_lighting_maps)
add_subdirectory(l12_light_casters)
