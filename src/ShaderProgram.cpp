#include <lgl/ShaderProgram.h>

#include <cassert>
#include <vector>

#include <glad/glad.h>

#include <lgl/Exception.h>
#include <lgl/Shader.h>

namespace {
void deleteProgram(unsigned int *program) {
    glDeleteProgram(*program);
    delete program;
}
} // namespace

namespace lgl {

ShaderProgram::ShaderProgram(const std::string &vertexShaderPathname,
                             const std::string &fragmentShaderPathname) :
    program(new unsigned int(glCreateProgram()), deleteProgram) {

    Shader vertexShader(vertexShaderPathname, GL_VERTEX_SHADER);
    Shader fragmentShader(fragmentShaderPathname, GL_FRAGMENT_SHADER);

    vertexShader.attachToShaderProgram(*this->program);
    fragmentShader.attachToShaderProgram(*this->program);
    glLinkProgram(*this->program);
    fragmentShader.detachFromShaderProgram(*this->program);
    vertexShader.detachFromShaderProgram(*this->program);

    int status;
    glGetProgramiv(*this->program, GL_LINK_STATUS, &status);
    if (!status) {
        int logLength;
        glGetProgramiv(*this->program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(*this->program, logLength, nullptr, log.data());
        throw BuildError(std::string(log.cbegin(), log.cend()));
    }
}

void ShaderProgram::use() const {
    glUseProgram(*this->program);
}

void ShaderProgram::setUniform(const std::string &name, float value) {
    auto location = glGetUniformLocation(*this->program, name.c_str());
    assert(("Failed to glGetUniformLocation()", location != -1));
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string &name, int value) {
    auto location = glGetUniformLocation(*this->program, name.c_str());
    assert(("Failed to glGetUniformLocation()", location != -1));
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string &name, const glm::vec4 &value) {
    auto location = glGetUniformLocation(*this->program, name.c_str());
    assert(("Failed to glGetUniformLocation()", location != -1));
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

} // namespace lgl
