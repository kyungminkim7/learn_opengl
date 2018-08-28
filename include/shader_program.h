#pragma once
#include <string>
#include <memory>

class ShaderProgram
{
public:
    static std::unique_ptr<ShaderProgram> New(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    void use();

    void setUniformBool(const std::string &name, bool value);
    void setUniformInt(const std::string &name, int value);
    void setUniformFloat(const std::string &name, float value);

private:
    ShaderProgram() = default;

    unsigned int id;
};
