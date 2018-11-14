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

private:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinate;
};

} // namespace gl
