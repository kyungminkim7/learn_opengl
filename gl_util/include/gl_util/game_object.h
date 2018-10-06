#pragma once

#include <chrono>

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

namespace gl {

///
/// \class GameObject
///
/// \brief The GameObject class represents an object in the 3D virtual world.
///
class GameObject {
public:
    virtual ~GameObject() = default;

    ///
    /// \brief onUpdate Updates the game object's state.
    ///
    /// This should be called on every iteration of the game loop.
    /// The base implementation does nothing.
    ///
    /// \param updateDuration Elapsed time since the last frame.
    ///
    virtual void onUpdate(std::chrono::duration<float> updateDuration);

    ///
    /// \brief onKeyInput Keyboard input controls.
    ///
    /// The base implementation does nothing.
    ///
    /// \param window The window that received the event.
    /// \param key The key that was pressed/released.
    /// \param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
    /// \param mods Bit field describing which modifier keys were held down.
    ///
    virtual void onKeyInput(GLFWwindow *window, int key, int action, int mods);

    ///
    /// \brief onCursorMoved Controls based on cursor movement.
    ///
    /// The base implementation does nothing.
    ///
    /// \param window The window that received the event.
    /// \param cursorX The new cursor x-coordinate, relative to the left edge of the client area.
    /// \param cursorY The new cursor y-coordinate, relative to the top edge of the client area.
    ///
    virtual void onCursorMoved(GLFWwindow *window, double cursorX, double cursorY);

    ///
    /// \brief onScrollInput Controls based on scrolling input.
    ///
    /// The base implementation does nothing.
    ///
    /// \param window The window that received the event
    /// \param xOffset The scroll offset along the x-axis.
    /// \param yOffset The scroll offset along the y-axis.
    ///
    virtual void onScrollInput(GLFWwindow *window, double xOffset, double yOffset);

    glm::mat4 getModelMatrix() const;

    GameObject& setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    GameObject& setOrientation(const glm::mat3& orientation);
    GameObject& setOrientation(const glm::vec3& orientationX,
                               const glm::vec3& orientationY,
                               const glm::vec3& orientationZ);
    glm::mat3 getOrientation() const;
    glm::vec3 getOrientationX() const;
    glm::vec3 getOrientationY() const;
    glm::vec3 getOrientationZ() const;

    ///
    /// \brief rotate Rotates the game object about an axis in the world coordinate frame.
    /// \param angle_rad Angle to rotate game object by (rad).
    /// \param axis Axis (in world coordinate frame) to rotate game object about.
    /// \return The game object to allow chaining of multiple rotation/translation calls.
    ///
    GameObject& rotate(float angle_rad, const glm::vec3& axis);

    ///
    /// \brief translate Translates the game object in the world coordinate frame.
    /// \param translation Amount to translate the game object by in the world coordinate frame.
    /// \return The game object to allow chaining of multiple rotation/translation calls.
    ///
    GameObject& translate(const glm::vec3& translation);

    ///
    /// \brief translateInBodyFrame Translates the game object in the local coordinate frame.
    /// \param translation Amount to translate the game object by in the local coordinate frame.
    /// \return The game object to allow chaining of multiple rotation/translation calls.
    ///
    GameObject& translateInLocalFrame(const glm::vec3& translation);

    ///
    /// \brief scale Assigns a new model scale.
    /// \param modelScale Scale of the model relative to original scal.
    /// \return The game object to allow chaining of multiple calls.
    ///
    GameObject& scale(const glm::vec3& modelScale);

private:
    glm::mat3 orientation {1.0f};
    glm::vec3 position {0.0f};
    glm::vec3 modelScale {1.0f};
};

inline glm::vec3 GameObject::getPosition() const {return this->position;}
inline glm::mat3 GameObject::getOrientation() const {return this->orientation;}
inline glm::vec3 GameObject::getOrientationX() const {return this->orientation[0];}
inline glm::vec3 GameObject::getOrientationY() const {return this->orientation[1];}
inline glm::vec3 GameObject::getOrientationZ() const {return this->orientation[2];}

} // namespace gl
