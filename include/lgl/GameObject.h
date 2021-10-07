#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Frame.h"
#include "Mesh.h"

namespace lgl {

class ShaderProgram;

class GameObject {
private:
    using Duration = std::chrono::duration<float>;

public:
    explicit GameObject(const std::string &pathname);

    glm::vec3 getPosition() const;
    glm::vec3 getOrientationX() const;
    glm::vec3 getOrientationY() const;
    glm::vec3 getOrientationZ() const;
    glm::mat4 getModelMatrix() const;

    void setPosition(const glm::vec3 &position);
    void translate(const glm::vec3 &translation);
    void translateInLocalFrame(const glm::vec3 &translation);
    void rotate(float angle_rad, const glm::vec3 &axis);
    void rotateInLocalFrame(float angle_rad, const glm::vec3 &axis);
    void lookAtPoint(const glm::vec3 &point);

    void onUpdate(Duration duration);
    void render(ShaderProgram *shaderProgram);

private:
    void processNode(const aiNode *node, const aiScene *scene);

    Frame frame;
    std::vector<Mesh> meshes;
};

inline glm::vec3 GameObject::getOrientationX() const { return this->frame.getOrientationX(); }
inline glm::vec3 GameObject::getOrientationY() const { return this->frame.getOrientationY(); }
inline glm::vec3 GameObject::getOrientationZ() const { return this->frame.getOrientationZ(); }
inline glm::vec3 GameObject::getPosition() const { return this->frame.getPosition(); }
inline glm::mat4 GameObject::getModelMatrix() const { return this->frame.getModelMatrix(); }

inline void GameObject::setPosition(const glm::vec3 &position) { this->frame.setPosition(position); }
inline void GameObject::translate(const glm::vec3 &translation) { this->frame.translate(translation); }
inline void GameObject::translateInLocalFrame(const glm::vec3 &translation) { this->frame.translateInLocalFrame(translation); }
inline void GameObject::rotate(float angle_rad, const glm::vec3 &axis) { this->frame.rotate(angle_rad, axis); }
inline void GameObject::rotateInLocalFrame(float angle_rad, const glm::vec3 &axis) { this->frame.rotateInLocalFrame(angle_rad, axis); }
inline void GameObject::lookAtPoint(const glm::vec3 &point) { this->frame.lookAtPoint(point); }

} // namespace lgl
