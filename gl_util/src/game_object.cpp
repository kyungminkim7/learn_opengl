#include <gl_util/game_object.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace gl {

GameObject::GameObject(const std::string& modelFilepath) : model(modelFilepath) {}

void GameObject::onUpdate(std::chrono::duration<float> updateDuration) {}
void GameObject::onKeyInput(GLFWwindow *window, int key, int action, int mods) {}
void GameObject::onCursorMoved(GLFWwindow *window, double cursorX, double cursorY) {}
void GameObject::onScrollInput(GLFWwindow *window, double xOffset, double yOffset) {}

} // namespace gl
