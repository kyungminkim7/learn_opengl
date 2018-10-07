#pragma once

#include <gl_util/game_object.h>

#include <chrono>

#include <GLFW/glfw3.h>

namespace gl {

///
/// \class Camera
///
/// \brief First person shooter camera with a perspective view.
///
/// This camera pitches and yaws but does not allow roll.
///
class Camera : public GameObject {
public:
    ///
    /// \brief Camera constructor.
    ///
    /// Sets the following default values:
    ///     1. mouse sensitivity  = 0.1
    ///     2. scroll sensitivity = 2.0
    ///     3. movement speed     = 10.0 m/s
    ///     4. rotation axis      = {0, 0, 1}
    ///     5. look at direction  = {1, 0, 0}
    ///     6. normal direction   = {0, 0, 1}
    ///     7. current fov        = max fov
    ///
    /// \param maxFov_deg Camera field of view in degrees.
    /// \param aspectRatioWidthToHeight
    /// \param nearPlane Distance to camera near plane in m.
    /// \param farPlane Distance to camera far plane in m.
    ///
    Camera(float maxFov_deg, float aspectRatioWidthToHeight, float nearPlane, float farPlane);

    ///
    /// \brief Updates the camera state.
    ///
    /// This should be called on every iteration of the game loop.
    ///
    /// \param updateDuration Elapsed time since the last frame.
    ///
    void onUpdate(std::chrono::duration<float> updateDuration) override;

    ///
    /// \brief Camera movement controls through keyboard input.
    ///
    /// Implements the following movement controls:
    ///     1. 'w' - forward
    ///     2. 's' - backward
    ///     3. 'a' - left
    ///     4. 'd' - right
    ///
    /// This movement speed may be modified through Camera::setMovementSpeed().
    ///
    /// \param window The window that received the event.
    /// \param key The key that was pressed/released.
    /// \param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
    /// \param mods Bit field describing which modifier keys were held down.
    ///
    void onKeyInput(GLFWwindow *window, int key, int action, int mods) override;

    ///
    /// \brief Controls camera through cursor movement.
    ///
    /// The camera rotates in place following the cursor movement.
    /// The sensitivity of this movement may be modified through Camera::setCursorSensitivity().
    ///
    /// \param window The window that received the event.
    /// \param cursorX The new cursor x-coordinate, relative to the left edge of the client area.
    /// \param cursorY The new cursor y-coordinate, relative to the top edge of the client area.
    ///
    void onCursorMoved(GLFWwindow *window, double cursorX, double cursorY) override;

    ///
    /// \brief onScrollInput Controls camera zoom through scrolling.
    /// \param window The window that received the event
    /// \param xOffset The scroll offset along the x-axis.
    /// \param yOffset The scroll offset along the y-axis.
    ///
    void onScrollInput(GLFWwindow *window, double xOffset, double yOffset) override;

    /// \name Look At
    ///
    /// Points the camera at a desired point or direction.
    ///
    /// If the new camera direction is linearly dependent with the camera's original normal,
    /// make sure to call Camera::setNormal...() afterwards to properly set the new orientation.
    ///@{
    void setLookAtPoint(const glm::vec3& lookAtPoint);
    void setLookAtDirection(const glm::vec3& lookAtDirection);
    ///@}

    glm::vec3 getLookAtDirection() const;

    /// \name Normal
    ///
    /// Sets the camera's normal.
    ///
    /// If the new camera's normal direction is linearly dependent with the camera's original
    /// look at direction, make sure to call Camera::setLookAt...() afterwards to properly
    /// set the new orientation.
    ///@{
    void setNormalDirection(const glm::vec3& normalDirection);
    ///@>

    glm::vec3 getNormalDirection() const;

    glm::mat4 getViewMatrix() const;

    void setMaxFov(float fov_deg);

    ///
    /// \brief Sets the camera's current field of view.
    ///
    /// The fov is capped between 1.0 and the supplied max field of view.
    ///
    /// \param fov_deg
    ///
    void setCurrentFov(float fov_deg);

    void setAspectRatioWidthToHeight(float aspectRatioWidthToHeight);

    glm::mat4 getProjectionMatrix() const;

    ///
    /// \brief Sets the speed of the camera's linear movements.
    /// \param linearSpeed Linear movement speed (m/s).
    ///
    void setLinearSpeed(float linearSpeed);

    ///
    /// \brief setCursorSenstivity Sets the sensitivity of the camera's rotation following
    ///                            the cursor's movements.
    /// \param cursorSensitivity
    ///
    void setCursorSenstivity(float cursorSensitivity);

    ///
    /// \brief setScrollSensitivity Sets the sensitivity of the camera's zoom through scrolling.
    /// \param scrollSensitivity
    ///
    void setScrollSensitivity(float scrollSensitivity);

    void setHorizontalRotationAxis(const glm::vec3& horizontalRotationAxis);

private:
    float maxFov_deg;
    float currentFov_deg;
    float aspectRatioWidthToHeight;
    float nearPlane; ///< m
    float farPlane;  ///< m

    float cursorSensitivity;
    float scrollSensitivity;

    float linearSpeed; ///< Speed of each linear velocity component (m/s)
    glm::vec3 linearVelocity {0.0f};

    glm::vec3 horizontalRotationAxis;

    glm::mat4 projectionMatrix {1.0f};
    glm::mat4 viewMatrix {1.0f};

    bool firstCursorPositionReceived = false;

    double lastCursorX = 0.0;
    double lastCursorY = 0.0;
};

inline glm::vec3 Camera::getLookAtDirection() const {return this->getOrientationX();}
inline glm::vec3 Camera::getNormalDirection() const {return this->getOrientationZ();}

} // namespace gl
