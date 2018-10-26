#include <gl_util/vertex.h>

namespace gl {

size_t Vertex::offsetOfPosition() noexcept {
    return offsetof(Vertex, position);
}

size_t Vertex::offsetOfNormal() noexcept {
    return offsetof(Vertex, normal);
}

size_t Vertex::offsetOfTextureCoordinate() noexcept {
    return offsetof(Vertex, textureCoordinate);
}

Vertex::Vertex(const glm::vec3& position,
               const glm::vec3& normal,
               const glm::vec2& textureCoordinate) :
    position(position), normal(normal),
    textureCoordinate(textureCoordinate) {}

} // namespace gl
