cmake_minimum_required(VERSION 3.0)
project(gl_util)

find_package(Boost REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(gl_util SHARED
    "src/shader_program.cpp"
    "src/glfw_util.cpp"
)

target_include_directories(gl_util
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(gl_util
    PUBLIC
        glad
        glfw
    PRIVATE
        ${Boost_LIBRARIES}
)

target_compile_features(gl_util
    PUBLIC
        cxx_nullptr
    PRIVATE
        cxx_auto_type
        cxx_constexpr
)
