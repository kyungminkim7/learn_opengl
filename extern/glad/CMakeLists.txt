cmake_minimum_required(VERSION 3.5...3.10)
project(glad VERSION 1.0.0)

# Create targets and set properties
add_library(${PROJECT_NAME} "src/glad.c")
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"    
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"    
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
