#pragma once

#include <vector>

#include "game_object.h"

namespace gl {

///
/// \brief The PointLight class represents a point light for use in generating
/// omnidirectional shadows.
///
class PointLight : public GameObject {
public:
    PointLight(float fov_deg, float aspectRatioWidthToHeight, float nearPlane, float farPlane);

    ///
    /// \brief getShadowTransforms Returns the 6 combinations of perspective projection
    /// and view matrices for each side of a cubemap for use in shadow mapping.
    /// \return 6 transformations for projecting shadows onto a cubemap.
    ///
    std::vector<glm::mat4> getShadowTransforms() const;

    float getNearPlane() const;
    float getFarPlane() const;

private:
    float fov_rad;
    float aspectRatioWidthToHeight;
    float nearPlane;
    float farPlane;
};

inline float PointLight::getNearPlane() const {return this->nearPlane;}
inline float PointLight::getFarPlane() const {return this->farPlane;}

} // namespace gl
