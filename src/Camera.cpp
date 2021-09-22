#include <lgl/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace lgl {

Camera::Camera(float fov_rad, float aspectRatio, float nearPlane, float farPlane) :
    fov_rad(fov_rad), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
    this->updateProjectionMatrix();
}

void Camera::updateProjectionMatrix() {
    this->projectionMatrix = glm::perspective(this->fov_rad, this->aspectRatio,
                                              this->nearPlane, this->farPlane);
}


} // namespace lgl
