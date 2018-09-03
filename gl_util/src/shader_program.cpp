#include "gl_util/shader_program.h"
#include <glad/glad.h>
#include <boost/optional.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace gl {

static constexpr unsigned int LOG_LENGTH = 1024;
static std::string readShaderFile(const std::string& shaderPath);
static boost::optional<unsigned int> compileShader(int shaderType, const char *shaderSource);

std::unique_ptr<ShaderProgram> ShaderProgram::New(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    std::unique_ptr<ShaderProgram> shaderProgram(new ShaderProgram);

    //Read shader codes from file
    std::ifstream vertexShaderFile(vertexShaderPath), fragmentShaderFile(fragmentShaderPath);

    if (!vertexShaderFile.is_open()) {
        std::cerr << "ERROR: Unable to open vertex shader file: " << vertexShaderPath << "\n";
        return nullptr;
    }
    std::cout << "Found vertex shader file: " << vertexShaderPath << "\n";

    if (!fragmentShaderFile.is_open()) {
        std::cerr << "ERROR: Unable to open fragment shader file: " << fragmentShaderPath << "\n";
        return nullptr;
    }
    std::cout << "Found fragment shader file: " << fragmentShaderPath << "\n";

    std::stringstream vertexShaderStream, fragmentShaderStream;
    vertexShaderStream << vertexShaderFile.rdbuf();
    fragmentShaderStream << fragmentShaderFile.rdbuf();

    auto vertexShaderCode = vertexShaderStream.str();
    auto fragmentShaderCode = fragmentShaderStream.str();

    //Compile shaders
    unsigned int vertexShader, fragmentShader;
    auto optVertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode.c_str());
    auto optFragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode.c_str());

    if (optVertexShader && optFragmentShader) {
        vertexShader = *optVertexShader;
        fragmentShader = *optFragmentShader;
        std::cout << "Successfully compiled vertex & fragment shaders.\n";
    } else {
        return nullptr;
    }

    //Link shaders
    shaderProgram->id = glCreateProgram();
    glAttachShader(shaderProgram->id, vertexShader);
    glAttachShader(shaderProgram->id, fragmentShader);

    int success;
    char log[LOG_LENGTH];
    glLinkProgram(shaderProgram->id);
    glGetProgramiv(shaderProgram->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram->id, LOG_LENGTH, nullptr, log);
        std::cerr << "ERROR: Shader program link failed:\n"
                  << log << "\n";
        return nullptr;
    }
    std::cout << "Successfully linked vertex & fragment shaders.\n";

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::string readShaderFile(const std::string& shaderPath) {

}

boost::optional<unsigned int> compileShader(int shaderType, const char *shaderSource) {
    //Compile shader
    auto shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    //Check for and display compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[LOG_LENGTH];
        glGetShaderInfoLog(shader, LOG_LENGTH, nullptr, log);

        std::string shaderTypeName;
        switch (shaderType) {
        case GL_VERTEX_SHADER:
            shaderTypeName = "Vertex";
            break;

        case GL_FRAGMENT_SHADER:
            shaderTypeName = "Fragment";
            break;

        default:
            break;
        }

        std::cerr << "ERROR: " << shaderTypeName << " shader compilation failed:\n"
                  << log << "\n";
        return boost::none;
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
