#pragma once

#include <string>

namespace gl {

///
/// \brief Texture data.
///
class Texture
{
public:
    Texture(const std::string& imageFilename, unsigned int id);

    std::string getImageFilename() const;
    unsigned int getId() const;

private:
    std::string imageFilename;
    unsigned int id;
};

inline std::string Texture::getImageFilename() const {return this->imageFilename;}
inline unsigned int Texture::getId() const {return this->id;}

} // namespace gl
