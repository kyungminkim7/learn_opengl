#include <lgl/ShaderProgram.h>

#include <vector>

#include <glad/glad.h>

#include <lgl/Exception.h>
#include <lgl/Shader.h>

namespace lgl {

ShaderProgram::ProgramDeleter ShaderProgram::programDeleter = [](auto program) {
    glDeleteProgram(*program);
    delete program;
};

ShaderProgram::ShaderProgram(const std::string &vertexShaderPathname,
                             const std::string &fragmentShaderPathname) :
    program(new unsigned int(glCreateProgram()), programDeleter) {

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

} // namespace lgl
