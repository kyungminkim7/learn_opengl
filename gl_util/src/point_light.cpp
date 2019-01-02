#include <gl_util/point_light.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

namespace gl {

PointLight::PointLight(float fov_deg, float aspectRatioWidthToHeight,
                       float nearPlane, float farPlane)
    : fov_rad(glm::radians(fov_deg)), aspectRatioWidthToHeight(aspectRatioWidthToHeight),
      nearPlane(nearPlane), farPlane(farPlane) {}

std::vector<glm::mat4> PointLight::getShadowTransforms() const {
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.reserve(6u);

    auto projection = glm::perspective(this->fov_rad, this->aspectRatioWidthToHeight,
                                       this->nearPlane, this->farPlane);
    auto position = this->getPosition();

    shadowTransforms.push_back(projection * glm::lookAt(position,
                                                        position + glm::vec3(1.0f, 0.0f, 0.0f),
                                                        {0.0f, -1.0f, 0.0f}));
    shadowTransforms.push_back(projection * glm::lookAt(position,
                                                        position + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                        {0.0f, -1.0f, 0.0f}));
    shadowTransforms.push_back(projection * glm::lookAt(position,
                                                        position + glm::vec3(0.0f, 1.0f, 0.0f),
                                                        {0.0f, 0.0f, 1.0f}));
    shadowTransforms.push_back(projection * glm::lookAt(position,
                                                        position + glm::vec3(0.0f, -1.0f, 0.0f),
                                                        {0.0f, 0.0f, -1.0f}));
    shadowTransforms.push_back(projection * glm::lookAt(position,
                                                        position + glm::vec3(0.0f, 0.0f, 1.0f),
                                                        {0.0f, -1.0f, 0.0f}));
    shadowTransforms.push_back(projection * glm::lookAt(position,
                                                        position + glm::vec3(0.0f, 0.0f, -1.0f),
                                                        {0.0f, -1.0f, 0.0f}));
    return shadowTransforms;
}


} // namespace gl
