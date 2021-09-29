#include <lgl/Camera.h>

#include <limits>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

namespace lgl {

Camera::Camera(float fov_rad, float aspectRatio, float nearPlane, float farPlane) :
    fov_rad(fov_rad), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
    this->updateProjectionMatrix();
}

float Camera::getFOV() const {
    return this->fov_rad;
}

void Camera::setFOV(float fov_rad) {
    this->fov_rad = fov_rad;
    this->updateProjectionMatrix();
}

void Camera::updateProjectionMatrix() {
    this->projectionMatrix = glm::perspective(this->fov_rad, this->aspectRatio,
                                              this->nearPlane, this->farPlane);
}

void Camera::onUpdate(Duration duration) {
    if (glm::length2(this->localLinearVelocity) > std::numeric_limits<float>::epsilon()) {
        this->translateInLocalFrame(this->localLinearVelocity * duration.count());
    }
}

void Camera::setLocalSpeedX(float speed) {
    this->localLinearVelocity.x = speed;
}

void Camera::setLocalSpeedY(float speed) {
    this->localLinearVelocity.y = speed;
}

void Camera::setLocalSpeedZ(float speed) {
    this->localLinearVelocity.z = speed;
}

} // namespace lgl
