#include "gl_util/shader_program.h"
#include <glad/glad.h>
#include <boost/optional.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

static constexpr unsigned int LOG_LENGTH = 512;
static boost::optional<unsigned int> compileShader(int shaderType, const char *shaderSource);

std::unique_ptr<ShaderProgram> ShaderProgram::New(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    std::unique_ptr<ShaderProgram> shaderProgram(new ShaderProgram);

    //Read shader codes from file
    std::ifstream vertexShaderFile(vertexShaderPath), fragmentShaderFile(fragmentShaderPath);
    std::stringstream vertexShaderStream, fragmentShaderStream;

    vertexShaderStream << vertexShaderFile.rdbuf();
    fragmentShaderStream << fragmentShaderFile.rdbuf();

    auto vertexShaderCode = vertexShaderStream.str().c_str();
    auto fragmentShaderCode = fragmentShaderStream.str().c_str();

    //Compile shaders
    unsigned int vertexShader, fragmentShader;
    auto optVertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
    auto optFragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    if (optVertexShader && optFragmentShader) {
        vertexShader = *optVertexShader;
        fragmentShader = *optFragmentShader;
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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

void ShaderProgram::setUniformBool(const std::string &name, bool value) {
    this->setUniformInt(name, value);
}

void ShaderProgram::setUniformInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void ShaderProgram::setUniformFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}
