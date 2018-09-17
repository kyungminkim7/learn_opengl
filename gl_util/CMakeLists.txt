cmake_minimum_required(VERSION 3.0)
project(gl_util)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(gl_util SHARED
    "src/glfw_util.cpp"
    "src/shader_program.cpp"
)

target_include_directories(gl_util
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(gl_util
    PUBLIC
        glad
        glfw
        glm
)

target_compile_features(gl_util
    PUBLIC
        cxx_nullptr
        cxx_noexcept
        cxx_override
    PRIVATE
        cxx_auto_type
        cxx_constexpr
)
