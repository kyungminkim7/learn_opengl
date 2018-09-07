#include <gl_util/shader_program.h>
#include <gl_util/gl_exception.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace gl {

static constexpr unsigned int LOG_LENGTH = 1024;

///Throws std::ios_base::failure if failed to read from file.
static std::string readFile(const std::string& filepath);

///Throws gl::BuildError if failed to compile shader.
static unsigned int compileShader(int shaderType, const std::string& shaderPath, const std::string& shaderCode);

ShaderProgram::ShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    auto vertexShaderCode = readFile(vertexShaderPath);
    auto fragmentShaderCode = readFile(fragmentShaderPath);

    //Compile shaders
    auto vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderPath, vertexShaderCode);
    auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath, fragmentShaderCode);
    std::cout << "Successfully compiled vertex & fragment shaders.\n";

    //Link shaders
    this->id = glCreateProgram();
    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);

    int success;
    glLinkProgram(this->id);
    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success) {
        char linkLog[LOG_LENGTH];
        glGetProgramInfoLog(this->id, LOG_LENGTH, nullptr, linkLog);

        std::stringstream errorMsg;
        errorMsg << "Failed to link shaders\n" << linkLog;
        throw gl::BuildError(errorMsg.str());
    }
    std::cout << "Successfully linked vertex & fragment shaders.\n";

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);

    //Check for valid file
    if (!file.is_open()) {
        std::stringstream errorMsg;
        errorMsg << "Failed to open file: " << filepath;
        throw std::ios_base::failure(errorMsg.str());
    }
    std::cout << "Found file: " << filepath << "\n";

    //Read file
    std::stringstream filestream;
    filestream << file.rdbuf();

    return filestream.str();
}

unsigned int compileShader(int shaderType, const std::string& shaderPath, const std::string& shaderCode) {
    //Compile shader
    auto shader = glCreateShader(shaderType);
    auto shaderCodeStr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCodeStr, nullptr);
    glCompileShader(shader);

    //Check for compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char compileLog[LOG_LENGTH];
        glGetShaderInfoLog(shader, LOG_LENGTH, nullptr, compileLog);

        std::stringstream errorMsg;
        errorMsg << "Failed to compile " << shaderPath << "\n" << compileLog;
        throw BuildError(errorMsg.str());
    }

    return shader;
}

void ShaderProgram::use() {
    glUseProgram(this->id);
}

void ShaderProgram::setUniform1b(const std::string &name, bool value) {
    this->setUniform1i(name, value);
}

void ShaderProgram::setUniform1i(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void ShaderProgram::setUniform1f(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void ShaderProgram::setUniform3f(const std::string &name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(this->id, name.c_str()), x, y, z);
}

void ShaderProgram::setUniform4f(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(this->id, name.c_str()), x, y, z, w);
}

}
