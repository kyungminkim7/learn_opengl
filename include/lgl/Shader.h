#pragma once

#include <memory>
#include <string>

namespace lgl {

class Shader {
public:
    Shader(const std::string &pathname, int type);
    void attachToShaderProgram(unsigned int program);
    void detachFromShaderProgram(unsigned int program);

private:
    std::unique_ptr<unsigned int, void(*)(unsigned int *)> shader;
};

} // namespace lgl
