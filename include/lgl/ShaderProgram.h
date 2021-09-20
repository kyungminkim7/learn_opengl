#pragma once

#include <functional>
#include <memory>
#include <string>

#include <glm/vec4.hpp>

namespace lgl {

class ShaderProgram {
public:
    ShaderProgram(const std::string &vertexShaderPathname,
                  const std::string &fragmentShaderPathname);

    void use() const;

    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, const glm::vec4 &value);

private:
    std::unique_ptr<unsigned int, void(*)(unsigned int *)> program;
};

} // namespace lgl
