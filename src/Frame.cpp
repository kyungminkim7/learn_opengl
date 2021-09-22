#include <lgl/Frame.h>

#include <glm/gtx/quaternion.hpp>

namespace lgl {

void Frame::translate(const glm::vec3 &translation) {
    this->position += translation;
}

void Frame::translateInLocalFrame(const glm::vec3 &translation) {
    this->translate(this->orientation * translation);
}

void Frame::rotate(float angle_rad, const glm::vec3 &axis) {
    this->orientation = glm::toMat3(glm::angleAxis(angle_rad, axis)) * this->orientation;
}

} // namespace lgl
