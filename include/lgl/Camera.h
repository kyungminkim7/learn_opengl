#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Frame.h"

namespace lgl {

class Camera {
public:
    Camera(float fov_rad, float aspectRatio, float nearPlane, float farPlane);
    glm::vec3 getPosition() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;

    void setPosition(const glm::vec3 &position);
    void translate(const glm::vec3 &translation);
    void translateInLocalFrame(const glm::vec3 &translation);
    void rotate(float angle_rad, const glm::vec3 &axis);

private:
    void updateProjectionMatrix();

    Frame frame;
    glm::mat4 projectionMatrix;
    float fov_rad;
    float aspectRatio;
    float nearPlane;
    float farPlane;
};

inline glm::mat4 Camera::getProjectionMatrix() const { return this->projectionMatrix; }
inline glm::vec3 Camera::getPosition() const { return this->frame.getPosition(); }
inline glm::mat4 Camera::getModelMatrix() const { return this->frame.getModelMatrix(); }
inline glm::mat4 Camera::getViewMatrix() const { return this->frame.getViewMatrix(); }

inline void Camera::setPosition(const glm::vec3 &position) { this->frame.setPosition(position); }
inline void Camera::translate(const glm::vec3 &translation) { this->frame.translate(translation); }
inline void Camera::translateInLocalFrame(const glm::vec3 &translation) { this->frame.translateInLocalFrame(translation); }
inline void Camera::rotate(float angle_rad, const glm::vec3 &axis) { this->frame.rotate(angle_rad, axis); }

} // namespace lgl
