#pragma once

#include <memory>
#include <string>

namespace lgl {

class Texture2D {
public:
    explicit Texture2D(const std::string &pathname);

    void bind();

private:
    std::unique_ptr<unsigned int, void(*)(unsigned int *)> texture;
};

} // namespace lgl
