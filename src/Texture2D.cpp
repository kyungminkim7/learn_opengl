#include <lgl/Texture2D.h>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <lgl/Exception.h>

namespace lgl {

Texture2D::TextureDeleter Texture2D::textureDeleter = [](auto texture) {
    glDeleteTextures(1, texture);
    delete texture;
};

Texture2D::Texture2D(const std::string &pathname) :
    texture(new unsigned int, textureDeleter) {

    glGenTextures(1, this->texture.get());

    stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    auto imgDeleter = [](auto imgData){ stbi_image_free(imgData); };
    std::unique_ptr<stbi_uc, decltype(imgDeleter)> imgData(
                stbi_load(pathname.c_str(), &width, &height, &numChannels, 0),
                imgDeleter);

    if (!imgData) {
        throw LoadError("Failed to load texture: " + pathname);
    }

    GLenum format;
    switch (numChannels) {
    case 1: format = GL_ALPHA; break;
    case 4: format = GL_RGBA; break;
    default: format = GL_RGB; break;
    }

    glBindTexture(GL_TEXTURE_2D, *this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, imgData.get());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, *this->texture);
}

} // namespace lgl
