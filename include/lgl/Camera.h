#pragma once

#include <glm/mat4x4.hpp>

#include "Frame.h"

namespace lgl {

class Camera {
public:
    Camera(float fov_rad, float aspectRatio, float nearPlane, float farPlane);
    glm::mat4 getProjectionMatrix() const;

private:
    void updateProjectionMatrix();

    Frame frame;
    glm::mat4 projectionMatrix;
    float fov_rad;
    float aspectRatio;
    float nearPlane;
    float farPlane;
};

inline glm::mat4 Camera::getProjectionMatrix() { return this->projectionMatrix; }

} // namespace lgl
