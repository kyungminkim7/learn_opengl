#include <gl_util/game_object.h>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gl {

void GameObject::onUpdate(std::chrono::duration<float> updateDuration) {}
void GameObject::onKeyInput(GLFWwindow *window, int key, int action, int mods) {}
void GameObject::onCursorMoved(GLFWwindow *window, double cursorX, double cursorY) {}
void GameObject::onScrollInput(GLFWwindow *window, double xOffset, double yOffset) {}

GameObject& GameObject::setPosition(const glm::vec3& position) {
    for (int i = 0; i < 3; ++i) {
        this->pose[3][i] = position[i];
    }
    return *this;
}

GameObject& GameObject::setOrientation(const glm::mat3& orientation) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            this->pose[i][j] = orientation[i][j];
        }
    }
    return *this;
}

GameObject& GameObject::setOrientation(const glm::vec3 &orientationX,
                                const glm::vec3 &orientationY,
                                const glm::vec3 &orientationZ) {
    int i;
    for (i = 0; i < 3; ++i) {
        this->pose[0][i] = orientationX[i];
    }

    for (i = 0; i < 3; ++i) {
        this->pose[1][i] = orientationY[i];
    }

    for (i = 0; i < 3; ++i) {
        this->pose[2][i] = orientationZ[i];
    }
    return *this;
}

GameObject& GameObject::rotate(float angle_rad, const glm::vec3 &axis) {
    this->setOrientation(glm::rotate(glm::mat4(1.0f), angle_rad, axis) * this->pose);
    return *this;
}

GameObject& GameObject::translate(const glm::vec3 &translation) {
    for (int i = 0; i < 3; ++i) {
        this->pose[3][i] += translation[i];
    }
    return *this;
}

GameObject& GameObject::translateInLocalFrame(const glm::vec3 &translation) {
    this->setPosition(this->pose * glm::vec4(translation, 1.0f));
    return *this;
}

} // namespace gl
