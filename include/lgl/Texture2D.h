#pragma once

#include <memory>
#include <string>

namespace lgl {

class Texture2D {
public:
    explicit Texture2D(const std::string &pathname);

    void bind();

private:
    std::shared_ptr<unsigned int> texture;
};

} // namespace lgl
