#pragma once

#include <functional>
#include <memory>
#include <string>

namespace lgl {

class Texture2D {
public:
    Texture2D(const std::string &pathname);

    void bind();

private:
    using TextureDeleter = std::function<void(unsigned int *)>;
    static TextureDeleter textureDeleter;

    std::unique_ptr<unsigned int, TextureDeleter> texture;
};

} // namespace lgl
