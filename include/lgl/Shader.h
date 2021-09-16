#pragma once

#include <functional>
#include <memory>
#include <string>

namespace lgl {

class Shader {
public:
    Shader(const std::string &pathname, int type);
    void attachToShaderProgram(unsigned int program);
    void detachFromShaderProgram(unsigned int program);

private:
    using ShaderDeleter = std::function<void(unsigned int *)>;
    static ShaderDeleter shaderDeleter;

    std::unique_ptr<unsigned int, ShaderDeleter> shader;
};

} // namespace lgl
