#pragma once

#include <memory>
#include <string>

namespace gl {

///
/// \brief Texture data.
///
class Texture
{
public:
    ///
    /// \brief loadTexture Loads and caches texture data from image file.
    ///
    /// Texture object will automatically clean up cache and GPU data on
    /// destruction. Do NOT call glDeleteTextures on this texture's id.
    ///
    /// \param imageFilepath Filepath to the image.
    /// \return OpenGL's texture ID for the loaded texture.
    /// \exception gl::LoadError Failed to load image data from file.
    ///
    explicit Texture(const std::string& imageFilepath);

    ///
    /// \brief bind Binds this texture to the GPU for rendering.
    ///
    void bind();

private:
    std::shared_ptr<unsigned int> id;
};

} // namespace gl
