#pragma once
#include <string>

#include <glm/fwd.hpp>

namespace gl {

///
/// \brief Manages loading, compiling, linking and working with shader programs.
///
class ShaderProgram
{
public:
    ///
    /// \brief Loads, compiles, and links given shaders into an OpenGL shader program.
    /// \param[in] vertexShaderPath Filepath of the vertex shader.
    /// \param[in] fragmentShaderPath Filepath of the fragment shader.
    /// \exception std::ios_base::failure Failed to open either file.
    /// \exception gl::BuildError Failed to compile or link shaders.
    ///
    ShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    ///
    /// \brief Deletes the OpenGL shader program from the GPU.
    ///
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram(ShaderProgram &&) = delete;
    ShaderProgram& operator=(const ShaderProgram &) = delete;
    ShaderProgram& operator=(ShaderProgram &&) = delete;

    ///
    /// \brief Sets this program as the current active shader program.
    ///
    /// This is a helper function that calls glUseProgram() on this shader program.
    ///
    ShaderProgram& use();

    /// \name Uniforms
    /// Sets uniform value on this shader program. User must call ShaderProgram::use() before
    /// the 1st call to a ShaderProgram::setUniform() function to ensure that they are
    /// setting the uniform on the right active shader program.
    ///@{
    ShaderProgram& setUniform(const std::string &name, bool value);
    ShaderProgram& setUniform(const std::string &name, int value);
    ShaderProgram& setUniform(const std::string &name, float value);
    ShaderProgram& setUniform(const std::string &name, float x, float y, float z);
    ShaderProgram& setUniform(const std::string &name, float x, float y, float z, float w);
    ShaderProgram& setUniform(const std::string &name, const glm::vec3 &v);
    ShaderProgram& setUniform(const std::string &name, const glm::mat3 &m);
    ShaderProgram& setUniform(const std::string &name, const glm::mat4 &m);
    ///@}

    ///
    /// \brief setUniformBlockBinding Links the uniform block of this shader to the binding point.
    /// \param uniformBlockName Name of this shader's uniform block to link.
    /// \param bindingPoint Binding point to link against.
    /// \return This shader program for convenient function chaining.
    ///
    ShaderProgram& setUniformBlockBinding(const std::string &uniformBlockName,
                                          unsigned int bindingPoint);

private:
    unsigned int id;
};

} // namespace gl
