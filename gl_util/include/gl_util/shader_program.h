#pragma once
#include <string>
#include <glm/glm.hpp>

namespace gl {

/**
 * @brief The ShaderProgram class manages loading, compiling, linking
 * and managing shader programs.
 */
class ShaderProgram
{
public:
    /**
     * @brief ShaderProgram Loads, compiles, and links given shaders into a shader program.
     *
     * Throws std::ios_base::failure if failed to open either file.
     * Throws gl::BuildError if failed to compile or link shaders.
     *
     * @param vertexShaderPath Filepath of the vertex shader.
     * @param fragmentShaderPath Filepath of the fragment shader.
     */
    ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    void use();

    void setUniform1b(const std::string &name, bool value);
    void setUniform1i(const std::string &name, int value);
    void setUniform1f(const std::string &name, float value);
    void setUniform3f(const std::string &name, float x, float y=0.0f, float z=0.0f);
    void setUniform4f(const std::string &name, float x, float y=0.0f, float z=0.0f, float w=0.0f);
    void setUniformMatrix4fv(const std::string &name, const glm::mat4 &m);

private:
    unsigned int id;
};

}
