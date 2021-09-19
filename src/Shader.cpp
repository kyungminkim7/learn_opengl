#include <lgl/Shader.h>

#include <iterator>
#include <fstream>
#include <vector>

#include <glad/glad.h>

#include <lgl/Exception.h>

namespace lgl {

Shader::ShaderDeleter Shader::shaderDeleter = [](auto shader) {
    glDeleteShader(*shader);
    delete shader;
};

Shader::Shader(const std::string &pathname, int type) :
    shader(new unsigned int(glCreateShader(type)), shaderDeleter) {

    // Read shader source file
    std::ifstream file(pathname);
    std::vector<char> shaderCode((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
    auto shaderCodePtr = &shaderCode[0];
    std::array<int, 1> shaderCodeLength{static_cast<int>(shaderCode.size())};

    // Compile shader
    glShaderSource(*this->shader, 1, &shaderCodePtr, &shaderCodeLength[0]);
    glCompileShader(*this->shader);

    int status;
    glGetShaderiv(*this->shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        int logLength = 0;
        glGetShaderiv(*this->shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(*this->shader, logLength, nullptr, log.data());
        throw BuildError(std::string(log.cbegin(), log.cend()));
    }
}

void Shader::attachToShaderProgram(unsigned int program) {
    glAttachShader(program, *this->shader);
}

void Shader::detachFromShaderProgram(unsigned int program) {
    glDetachShader(program, *this->shader);
}

} // namespace lgl
