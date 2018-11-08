#include <gl_util/texture.h>

#include <unordered_map>

#include <glad/glad.h>
#include <stb_image.h>

#include <gl_util/gl_exception.h>
#include <iostream>

namespace {

std::unordered_map<std::string, std::weak_ptr<unsigned int>> cachedTextureIds;

///
/// \brief loadTexture Loads and caches texture data from image file.
/// \param imageFilepath Filepath to the image.
/// \return OpenGL's texture ID for the loaded texture.
/// \exception gl::LoadError Failed to load image data from file.
///
std::shared_ptr<unsigned int> loadTexture(const std::string& imageFilepath) {
    const auto imageFilename = imageFilepath.substr(imageFilepath.find_last_of('/') + 1);

    // Check cache to avoid reloading
    auto textureId = cachedTextureIds[imageFilename].lock();
    if (textureId) return textureId;

    // Load image from file
    int width, height, numChannels;
    auto data = stbi_load(imageFilepath.c_str(), &width, &height, &numChannels, 0);

    if (!data) {
        stbi_image_free(data);
        throw gl::LoadError("Failed to load texture at: " + imageFilepath);
    }

    GLenum format;
    switch (numChannels) {
    case 1:
        format = GL_RED;
        break;

    case 3:
        format = GL_RGB;
        break;

    case 4:
        format = GL_RGBA;
        break;

    default:
        format = GL_RGB;
        break;
    }

    // Clean up texture data on GPU and clear cache
    auto textureIdDeleter = [imageFilename](auto textureId) {
        glDeleteTextures(1, textureId);

        cachedTextureIds.erase(imageFilename);
        delete textureId;
    };
    textureId = std::shared_ptr<unsigned int>(new unsigned int, textureIdDeleter);

    // Load texture data onto GPU
    glGenTextures(1, textureId.get());
    glBindTexture(GL_TEXTURE_2D, *textureId);

    glTexImage2D(GL_TEXTURE_2D,
                 0, static_cast<GLint>(format), width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    cachedTextureIds[imageFilename] = textureId;

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return textureId;
}

} // namespace

namespace gl {

Texture::Texture(const std::string& imageFilepath)
    : id(loadTexture(imageFilepath)) {}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, *this->id);
}

} // namespace gl
