#include <lgl/Vertex.h>

#include <utility>

namespace lgl {

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoordinates) :
    position(std::move(position)),
    normal(std::move(normal)),
    textureCoordinates(std::move(textureCoordinates)) {}

} // namespace
