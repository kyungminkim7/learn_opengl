#pragma once

#include <gl_util/game_object.h>

#include <chrono>

namespace gl {

///
/// \brief The DirectionalLight class represents a directional light with uniform
/// orthonormal projection.
///
class DirectionalLight : public GameObject {
public:
    ///
    /// \brief DirectionalLight Defines the limits of the light's view.
    /// \param left
    /// \param right
    /// \param bottom
    /// \param top
    /// \param nearPlane
    /// \param farPlane
    ///
    DirectionalLight(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    ///
    /// \brief getProjectionMatrix returns the orthonormal projection matrix of the light.
    /// \return The orthonormal projection matrix of the light.
    ///
    glm::mat4 getProjectionMatrix() const;

private:
    float left;
    float right;
    float bottom;
    float top;
    float nearPlane;
    float farPlane;
};

} // namespace gl
