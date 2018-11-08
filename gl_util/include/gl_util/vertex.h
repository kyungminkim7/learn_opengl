#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace gl {

///
/// \brief Vertex attribute data.
///
class Vertex
{
public:
    /// \name Vertex Attribute Byte Offsets
    ///
    /// Provides the offset of the vertex attributes in bytes for use
    /// with glVertexAttribPointer().
    ///@{
    static size_t offsetOfPosition() noexcept;
    static size_t offsetOfNormal() noexcept;
    static size_t offsetOfTextureCoordinate() noexcept;
    ///@}

    Vertex(const glm::vec3 &position,
           const glm::vec3 &normal,
           const glm::vec2 &textureCoordinate);

    void setPosition(const glm::vec3 &position);
    glm::vec3 getPosition() const;

    void setNormal(const glm::vec3 &normal);
    glm::vec3 getNormal() const;

    void setTextureCoordinate(const glm::vec2 &textureCoordinate);
    glm::vec2 getTextureCoordinate() const;

private:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinate;
};

inline void Vertex::setPosition(const glm::vec3& position) {this->position = position;}
inline glm::vec3 Vertex::getPosition() const {return this->position;}

inline void Vertex::setNormal(const glm::vec3& normal) {this->normal = normal;}
inline glm::vec3 Vertex::getNormal() const {return this->normal;}

inline void Vertex::setTextureCoordinate(const glm::vec2& textureCoordinate) {this->textureCoordinate = textureCoordinate;}
inline glm::vec2 Vertex::getTextureCoordinate() const {return this->textureCoordinate;}

} // namespace gl
