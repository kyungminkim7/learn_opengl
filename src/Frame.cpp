#include <lgl/Frame.h>

#include <glm/gtx/quaternion.hpp>

namespace lgl {

glm::mat4 Frame::getModelMatrix() const {
    glm::mat4 modelMatrix(this->orientation);
    for (auto i = 0; i < 3; ++i) {
        modelMatrix[3][i] = this->position[i];
    }
    return glm::scale(modelMatrix, this->scale);
}

void Frame::setScale(const glm::vec3 &scale) {
    this->scale = scale;
}

void Frame::setPostion(const glm::vec3 &position) {
    this->position = position;
}

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
