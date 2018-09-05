cmake_minimum_required(VERSION 3.0)
project(stb)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_compile_definitions(${PROJECT_NAME}
    INTERFACE
        STB_IMAGE_IMPLEMENTATION
)
