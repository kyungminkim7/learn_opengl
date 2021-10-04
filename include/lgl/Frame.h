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
    glm::vec3 getOrientationX() const;
    glm::vec3 getOrientationY() const;
    glm::vec3 getOrientationZ() const;
    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    glm::mat3 getNormalMatrix() const;

    void setScale(const glm::vec3 &scale);
    void setPosition(const glm::vec3 &position);
    void translate(const glm::vec3 &translation);
    void translateInLocalFrame(const glm::vec3 &translation);
    void rotate(float angle_rad, const glm::vec3 &axis);
    void rotateInLocalFrame(float angle_rad, const glm::vec3 &axis);
    void lookAtPoint(const glm::vec3 &point);

private:
    glm::vec3 scale{1.0f};
    glm::vec3 position{0.0f};
    glm::mat3 orientation{1.0f};

    mutable bool normalMatrixIsValid = true;
    mutable glm::mat3 normalMatrix{1.0f};
};

inline glm::vec3 Frame::getScale() const { return this->scale; }
inline glm::vec3 Frame::getPosition() const { return this->position; }
inline glm::mat3 Frame::getOrientation() const { return this->orientation; }
inline glm::vec3 Frame::getOrientationX() const { return this->orientation[0]; }
inline glm::vec3 Frame::getOrientationY() const { return this->orientation[1]; }
inline glm::vec3 Frame::getOrientationZ() const { return this->orientation[2]; }

} // namespace lgl
