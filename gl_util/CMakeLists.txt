cmake_minimum_required(VERSION 3.0)
project(gl_util)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(gl_util SHARED
    "src/camera.cpp"
    "src/game_object.cpp"
    "src/glfw_util.cpp"
    "src/mesh.cpp"
    "src/model.cpp"
    "src/shader_program.cpp"
    "src/texture.cpp"
    "src/uniform_buffer.cpp"
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
    PRIVATE
        assimp
        stb
)

target_compile_features(gl_util
    PUBLIC
        cxx_constexpr
        cxx_noexcept
        cxx_nullptr        
        cxx_override
        cxx_uniform_initialization
    PRIVATE
        cxx_auto_type
        cxx_generic_lambdas
        cxx_range_for
)
