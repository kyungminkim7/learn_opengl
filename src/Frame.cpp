#include <lgl/Frame.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace lgl {

glm::mat4 Frame::getModelMatrix() const {
    glm::mat4 modelMatrix(this->orientation);
    for (auto i = 0; i < 3; ++i) {
        modelMatrix[3][i] = this->position[i];
    }
    return glm::scale(modelMatrix, this->scale);
}

glm::mat4 Frame::getViewMatrix() const {
    return glm::lookAt(this->position,
                       this->position + this->orientation[0],
                       this->orientation[2]);
}

void Frame::setScale(const glm::vec3 &scale) {
    this->scale = scale;
}

void Frame::setPosition(const glm::vec3 &position) {
    this->position = position;
}

void Frame::translate(const glm::vec3 &translation) {
    this->position += translation;
}

void Frame::translateInLocalFrame(const glm::vec3 &translation) {
    this->translate(this->orientation * translation);
}

void Frame::rotate(float angle_rad, const glm::vec3 &axis) {
    this->orientation = glm::mat3_cast(glm::rotate(glm::quat_cast(this->orientation),
                                                   angle_rad, axis));
}

void Frame::lookAtPoint(const glm::vec3 &point) {
    this->orientation[0] = glm::normalize(point - this->position);
    this->orientation[1] = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), this->orientation[0]);
    this->orientation[2] = glm::cross(this->orientation[0], this->orientation[1]);
}

} // namespace lgl
