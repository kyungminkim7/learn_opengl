#pragma once
#include <string>
#include <memory>

namespace gl {

/**
 * @brief The ShaderProgram class manages loading, compiling, linking
 * and managing shader programs.
 */
class ShaderProgram
{
public:
    /**
     * @brief New Factory function for loading, compiling, linking shaders
     * to create a shader program.
     *
     * @param vertexShaderPath Vertex shader filepath.
     * @param fragmentShaderPath Vertex shader filepath.
     * @return Shader program if successfully compiled and linked;
     *         nullptr if compilation or link failed.
     */
    static std::unique_ptr<ShaderProgram> New(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

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

private:
    ShaderProgram() = default;

    unsigned int id;
};

}
