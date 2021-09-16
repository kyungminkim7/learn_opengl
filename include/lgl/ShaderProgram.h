#pragma once

#include <functional>
#include <memory>
#include <string>

namespace lgl {

class ShaderProgram {
public:
    ShaderProgram(const std::string &vertexShaderPathname,
                  const std::string &fragmentShaderPathname);

    void use() const;

private:
    using ProgramDeleter = std::function<void(unsigned int *)>;
    static ProgramDeleter programDeleter;

    std::unique_ptr<unsigned int, ProgramDeleter> program;
};

} // namespace lgl
