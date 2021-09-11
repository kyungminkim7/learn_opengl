cmake_minimum_required(VERSION 3.5...3.10)

add_subdirectory(glad)

add_subdirectory(glfw)

add_subdirectory(glm)

add_library(stb stb/stb_vorbis.c)
add_library(stb::stb ALIAS stb)
target_include_directories(stb
   PUBLIC
       "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
       "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
       "$<INSTALL_INTERFACE:>"
   PRIVATE
       "${CMAKE_CURRENT_SOURCE_DIR}/stb"
)
target_compile_definitions(stb PUBLIC STB_IMAGE_IMPLEMENTATION)
