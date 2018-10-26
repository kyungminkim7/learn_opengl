#include <gl_util/texture.h>

namespace gl {

Texture::Texture(const std::string& imageFilename, unsigned int id)
    : imageFilename(imageFilename), id(id) {}

} // namespace gl
