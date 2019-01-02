#pragma once

#include <chrono>
#include <memory>
#include <unordered_set>
#include <vector>

#include <assimp/scene.h>

#include "model.h"

namespace gl {

class InstancingMesh;

///
/// \brief The InstancingGameObjects class allows drawing lots of models
/// sharing the same vertex data and using instanced rendering.
///
class InstancingGameObjects
{
public:
    class InstancingModel;
    using ModelContainer = std::vector<InstancingModel>;
    using iterator = ModelContainer::iterator;
    using const_iterator = ModelContainer::const_iterator;
    using reference = ModelContainer::reference;
    using const_reference = ModelContainer::const_reference;

    ///
    /// \brief InstancingGameObjects Creates a number of game objects
    ///                              sharing the same mesh data.
    /// \param modelFilepath Filepath to the model data.
    /// \param count Number of game object instances to create.
    /// \exception gl::LoadError Failed to load mesh data from model file.
    /// \exception gl::LoadError Failed to load texture image from file.
    ///
    explicit InstancingGameObjects(const std::string& modelFilepath, size_t count);
    virtual ~InstancingGameObjects();

    virtual void onUpdate(std::chrono::duration<float> updateDuration);

    void render(ShaderProgram *shader);

    /// \name Member Access
    /// Allows accessing individual models to change or access
    /// model data such as pose and scale.
    ///@{
    reference operator[](size_t pos);
    const_reference operator[](size_t pos) const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;
    ///@}

    size_t size() const noexcept;

private:
    using Meshes = std::vector<std::unique_ptr<gl::InstancingMesh>>;

    ///
    /// \brief loadMeshes Loads and caches mesh data from model file.
    ///
    /// Mesh caches will automatically be cleaned up as the last
    /// reference to the returned shared_ptr<Meshes> is destroyed.
    ///
    /// \param modelFilepath Absolute filepath to the model data.
    /// \exception gl::LoadError Failed to load mesh data from model file.
    /// \exception gl::LoadError Failed to load texture image from file.
    ///
    void loadMeshes(const std::string &modelFilepath);
    void processNode(const aiNode &node, const aiScene &scene, const std::string &modelDirectory);

    ModelContainer models;
    std::unordered_set<size_t> changedModelIndices;

    unsigned int modelMatrixBufferObject;
    unsigned int normalMatrixBufferObject;
    std::shared_ptr<Meshes> meshes;
};

///
/// \brief The InstancingGameObjects::InstancingModel class is a proxy class
///        for Model to allow notifying the owning InstancingGameObject when
///        model data has been changed for optimal update speeds.
///
class InstancingGameObjects::InstancingModel {
public:
    explicit InstancingModel(InstancingGameObjects &parentGameObject, size_t idx);

    glm::mat4 getModelMatrix() const;
    glm::mat3 getNormalMatrix() const;

    InstancingModel& setPosition(const glm::vec3 &position);
    glm::vec3 getPosition() const;

    InstancingModel& setOrientation(const glm::mat3 &orientation);
    InstancingModel& setOrientation(const glm::vec3 &orientationX,
                                  const glm::vec3 &orientationY,
                                  const glm::vec3 &orientationZ);
    glm::mat3 getOrientation() const;
    glm::vec3 getOrientationX() const;
    glm::vec3 getOrientationY() const;
    glm::vec3 getOrientationZ() const;

    InstancingModel& rotate(float angle_rad, const glm::vec3 &axis);

    InstancingModel& translate(const glm::vec3 &translation);
    InstancingModel& translateInLocalFrame(const glm::vec3 &translation);

    InstancingModel& setScale(const glm::vec3 &scale);

private:
    InstancingModel& notifyModelChanged();

    InstancingGameObjects &parentGameObject;
    size_t idx;
    Model model;
};

inline InstancingGameObjects::reference InstancingGameObjects::operator[](size_t pos) {
    return this->models[pos];
}

inline InstancingGameObjects::const_reference InstancingGameObjects::operator[](size_t pos) const {
    return this->models[pos];
}

inline InstancingGameObjects::iterator InstancingGameObjects::begin() noexcept {
    return this->models.begin();
}

inline InstancingGameObjects::const_iterator InstancingGameObjects::begin() const noexcept {
    return this->models.begin();
}

inline InstancingGameObjects::const_iterator InstancingGameObjects::cbegin() const noexcept {
    return this->models.cbegin();
}

inline InstancingGameObjects::iterator InstancingGameObjects::end() noexcept {
    return this->models.end();
}

inline InstancingGameObjects::const_iterator InstancingGameObjects::end() const noexcept {
    return this->models.end();
}

inline InstancingGameObjects::const_iterator InstancingGameObjects::cend() const noexcept {
    return this->models.cend();
}

inline size_t InstancingGameObjects::size() const noexcept {
    return this->models.size();
}

inline glm::mat4 InstancingGameObjects::InstancingModel::getModelMatrix() const {
    return this->model.getModelMatrix();
}

inline glm::mat3 InstancingGameObjects::InstancingModel::getNormalMatrix() const {
    return this->model.getNormalMatrix();
}

inline glm::vec3 InstancingGameObjects::InstancingModel::getPosition() const {
    return this->model.getPosition();
}

inline glm::mat3 InstancingGameObjects::InstancingModel::getOrientation() const {
    return this->model.getOrientation();
}

inline glm::vec3 InstancingGameObjects::InstancingModel::getOrientationX() const {
    return this->model.getOrientationX();
}

inline glm::vec3 InstancingGameObjects::InstancingModel::getOrientationY() const {
    return this->model.getOrientationY();
}

inline glm::vec3 InstancingGameObjects::InstancingModel::getOrientationZ() const {
    return this->model.getOrientationZ();
}

} // namespace gl
