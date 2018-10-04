#define GLM_ENABLE_EXPERIMENTAL

#include <gl_util/camera.h>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

namespace {
constexpr float MIN_FOV_DEG = 1.0f;
}

namespace gl {
Camera::Camera(float maxFov_deg, float aspectRatioWidthToHeight, float nearPlane, float farPlane) :
    maxFov_deg(maxFov_deg), currentFov_deg(maxFov_deg), aspectRatioWidthToHeight(aspectRatioWidthToHeight),
    nearPlane(nearPlane), farPlane(farPlane),
    cursorSensitivity(0.1f), scrollSensitivity(2.0f), linearSpeed(10.0f),
    horizontalRotationAxis(0.0f, 0.0f, 1.0f),
    lastUpdateTimepoint(std::chrono::system_clock::now()) {}

void Camera::onUpdate() {
    auto currentTimepoint = std::chrono::system_clock::now();

    using sec_f = std::chrono::duration<float>;
    auto updateTime = std::chrono::duration_cast<sec_f>(currentTimepoint - lastUpdateTimepoint).count();

    if (glm::length2(this->linearVelocity) > 0.0f) {
        this->setPosition(this->pose * glm::vec4(this->linearVelocity * updateTime, 1.0f));
    }

    this->lastUpdateTimepoint = currentTimepoint;
}

void Camera::onKeyInput(GLFWwindow *window, int key, int action, int mods) {
    auto forwardKeyStatus = glfwGetKey(window, GLFW_KEY_W);
    auto backwardKeyStatus = glfwGetKey(window, GLFW_KEY_S);
    auto leftKeyStatus = glfwGetKey(window, GLFW_KEY_A);
    auto rightKeyStatus = glfwGetKey(window, GLFW_KEY_D);

    if (forwardKeyStatus == GLFW_PRESS && backwardKeyStatus == GLFW_RELEASE) {
        this->linearVelocity.x = linearSpeed;
    }

    if (backwardKeyStatus == GLFW_PRESS && forwardKeyStatus == GLFW_RELEASE) {
        this->linearVelocity.x = -linearSpeed;
    }

    if (leftKeyStatus == GLFW_PRESS && rightKeyStatus == GLFW_RELEASE) {
        this->linearVelocity.y = linearSpeed;
    }

    if (rightKeyStatus == GLFW_PRESS && leftKeyStatus == GLFW_RELEASE) {
        this->linearVelocity.y = -linearSpeed;
    }

    if (forwardKeyStatus == GLFW_RELEASE && backwardKeyStatus == GLFW_RELEASE) {
        this->linearVelocity.x = 0;
    }

    if (leftKeyStatus == GLFW_RELEASE && rightKeyStatus == GLFW_RELEASE) {
        this->linearVelocity.y = 0;
    }
}

void Camera::onCursorMoved(GLFWwindow *window, double cursorX, double cursorY) {
    if (!firstCursorPositionReceived) {
        this->lastCursorX = cursorX;
        this->lastCursorY = cursorY;
        firstCursorPositionReceived = true;
    }

    auto xOffset = cursorX - this->lastCursorX;
    auto yOffset = cursorY - this->lastCursorY;

    auto deltaYaw = static_cast<float>(glm::radians(-xOffset * cursorSensitivity));
    auto deltaPitch = static_cast<float>(glm::radians(yOffset * cursorSensitivity));

    auto position = this->getPosition();
    this->pose = glm::rotate(glm::rotate(glm::mat4(1.0f), deltaYaw, this->horizontalRotationAxis),
                             deltaPitch, this->getOrientationY()) * this->pose;
    this->setPosition(position);

    this->lastCursorX = cursorX;
    this->lastCursorY = cursorY;
}

void Camera::onScrollInput(GLFWwindow *window, double xOffset, double yOffset) {
    this->setCurrentFov(this->currentFov_deg - this->scrollSensitivity * yOffset);
}

void Camera::setPosition(const glm::vec3& position) {
    this->pose[3][0] = position.x;
    this->pose[3][1] = position.y;
    this->pose[3][2] = position.z;

    this->viewMatrixValid = false;
}

glm::vec3 Camera::getPosition() const {return glm::column(this->pose, 3);}

void Camera::setLookAtPoint(const glm::vec3& lookAtPoint) {
    this->setLookAtDirection(lookAtPoint - this->getPosition());
}

void Camera::setLookAtDirection(const glm::vec3 &lookAtDirection) {
    auto orientationX = glm::normalize(lookAtDirection);
    auto orientationY = glm::normalize(glm::cross(this->getOrientationZ(), orientationX));
    auto orientationZ = glm::normalize(glm::cross(orientationX, orientationY));

    this->setOrientation(orientationX, orientationY, orientationZ);
}

void Camera::setNormalDirection(const glm::vec3 &normalDirection) {
    auto orientationZ = glm::normalize(normalDirection);
    auto orientationY = glm::normalize(glm::cross(orientationZ, this->getOrientationX()));
    auto orientationX = glm::normalize(glm::cross(orientationY, orientationZ));

    this->setOrientation(orientationX, orientationY, normalDirection);
}

glm::mat4 Camera::getViewMatrix() const {
    if (!this->viewMatrixValid) {
        this->viewMatrix = glm::lookAt(this->getPosition(),
                                       this->getPosition() + this->getOrientationX(),
                                       this->getOrientationZ());
        this->viewMatrixValid = true;
    }

    return this->viewMatrix;
}

void Camera::setMaxFov(float fov_deg) {
    this->maxFov_deg = fov_deg;
}

void Camera::setCurrentFov(float fov_deg) {
    if (fov_deg < MIN_FOV_DEG) {
        this->currentFov_deg = MIN_FOV_DEG;
    } else if (fov_deg > this->maxFov_deg) {
        this->currentFov_deg = this->maxFov_deg;
    } else {
        this->currentFov_deg = fov_deg;
    }

    this->projectionMatrixValid = false;
}

glm::mat4 Camera::getProjectionMatrix() const {
    if (!this->projectionMatrixValid) {
        this->projectionMatrix = glm::perspective(glm::radians(currentFov_deg),
                                                  aspectRatioWidthToHeight,
                                                  nearPlane, farPlane);
        this->projectionMatrixValid = true;
    }

    return this->projectionMatrix;
}

void Camera::setLinearSpeed(float linearSpeed) {
    this->linearSpeed = (linearSpeed < 0.0f ? 0.0f : linearSpeed);
}

void Camera::setCursorSenstivity(float cursorSensitivity) {
    this->cursorSensitivity = cursorSensitivity;
}

void Camera::setScrollSensitivity(float scrollSensitivity) {
    this->scrollSensitivity = scrollSensitivity;
}

void Camera::setHorizontalRotationAxis(const glm::vec3& yawAxis) {
    this->horizontalRotationAxis = yawAxis;
}

void Camera::setOrientation(const glm::vec3 &orientationX,
                            const glm::vec3 &orientationY,
                            const glm::vec3 &orientationZ) {
    this->pose[0][0] = orientationX.x;
    this->pose[0][1] = orientationX.y;
    this->pose[0][2] = orientationX.z;

    this->pose[1][0] = orientationY.x;
    this->pose[1][1] = orientationY.y;
    this->pose[1][2] = orientationY.z;

    this->pose[2][0] = orientationZ.x;
    this->pose[2][1] = orientationZ.y;
    this->pose[2][2] = orientationZ.z;

    this->viewMatrixValid = false;
}

glm::vec3 Camera::getOrientationX() const {return glm::column(this->pose, 0);}
glm::vec3 Camera::getOrientationY() const {return glm::column(this->pose, 1);}
glm::vec3 Camera::getOrientationZ() const {return glm::column(this->pose, 2);}

} // namespace gl
