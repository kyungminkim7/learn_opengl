#pragma once

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace lgl {

class Frame {
public:
    glm::vec3 getScale() const;
    glm::vec3 getPosition() const;
    glm::mat3 getOrientation() const;
    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;

    void setScale(const glm::vec3 &scale);
    void setPostion(const glm::vec3 &position);
    void translate(const glm::vec3 &translation);
    void translateInLocalFrame(const glm::vec3 &translation);
    void rotate(float angle_rad, const glm::vec3 &axis);

private:
    glm::vec3 scale{1.0f};
    glm::vec3 position{0.0f};
    glm::mat3 orientation{1.0f};
};

inline glm::vec3 Frame::getScale() const { return this->scale; }
inline glm::vec3 Frame::getPosition() const { return this->position; }
inline glm::mat3 Frame::getOrientation() const { return this->orientation; }

} // namespace lgl
