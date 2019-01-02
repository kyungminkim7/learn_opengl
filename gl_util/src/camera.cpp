#define GLM_ENABLE_EXPERIMENTAL

#include <gl_util/camera.h>

#include <glm/gtx/norm.hpp>

namespace {
constexpr float MIN_FOV_DEG = 1.0f;
}

namespace gl {
Camera::Camera(float maxFov_deg, float aspectRatioWidthToHeight, float nearPlane, float farPlane)
    : GameObject(),
      maxFov_deg(maxFov_deg), currentFov_deg(maxFov_deg), aspectRatioWidthToHeight(aspectRatioWidthToHeight),
      nearPlane(nearPlane), farPlane(farPlane),
      cursorSensitivity(0.1f), scrollSensitivity(2.0f), linearSpeed(10.0f),
      horizontalRotationAxis(0.0f, 0.0f, 1.0f) {}

void Camera::onUpdate(std::chrono::duration<float> updateDuration) {
    if (glm::length2(this->linearVelocity) > 0.0f) {
        this->translateInLocalFrame(this->linearVelocity * updateDuration.count());
    }
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

    this->rotate(deltaPitch, this->getOrientationY())
            .rotate(deltaYaw, this->horizontalRotationAxis);

    this->lastCursorX = cursorX;
    this->lastCursorY = cursorY;
}

void Camera::onScrollInput(GLFWwindow *window, double xOffset, double yOffset) {
    this->setCurrentFov(static_cast<float>(this->currentFov_deg -
                                           this->scrollSensitivity * yOffset));
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
}

void Camera::setAspectRatioWidthToHeight(float aspectRatioWidthToHeight) {
    this->aspectRatioWidthToHeight = aspectRatioWidthToHeight;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(currentFov_deg),
                            aspectRatioWidthToHeight,
                            nearPlane, farPlane);
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

void Camera::setHorizontalRotationAxis(const glm::vec3 &yawAxis) {
    this->horizontalRotationAxis = yawAxis;
}

} // namespace gl
