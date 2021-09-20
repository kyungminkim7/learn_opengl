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
    using ProgramDeleter = std::function<void(unsigned int *)>;
    static ProgramDeleter programDeleter;

    std::unique_ptr<unsigned int, ProgramDeleter> program;
};

} // namespace lgl
