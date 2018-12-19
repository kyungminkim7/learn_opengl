#include <gl_util/instancing_game_objects.h>

#include <iostream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>

#include <gl_util/gl_exception.h>

namespace {

using Meshes = std::vector<std::unique_ptr<gl::InstancingMesh>>;
std::unordered_map<std::string, std::weak_ptr<Meshes>> cachedMeshes;

} // namespace

namespace gl {

/// ----------------------------------------------------
///            InstancingGameObjects Functions
/// ----------------------------------------------------
InstancingGameObjects::InstancingGameObjects(const std::string& modelFilepath, size_t count) {
    this->models.reserve(count);
    for (auto i = 0ul; i < count; ++i) {
        this->models.emplace_back(*this, i);
    }

    glGenBuffers(1, &this->modelMatrixBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->modelMatrixBufferObject);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &this->normalMatrixBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->normalMatrixBufferObject);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->loadMeshes(modelFilepath);
}

void InstancingGameObjects::loadMeshes(const std::string &modelFilepath) {
    auto modelFilenameIndex = modelFilepath.find_last_of('/');
    const auto modelDirectory = modelFilepath.substr(0, modelFilenameIndex);
    const auto modelFilename = modelFilepath.substr(modelFilenameIndex + 1);

    // Check cached meshes to avoid reloading
    this->meshes = cachedMeshes[modelFilename].lock();
    if (this->meshes) return;

    // Load meshes from file
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(modelFilepath,
                                         aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw LoadError(importer.GetErrorString());
    }

    auto meshesDeleter = [modelFilename](auto meshes){
        // Clear cache
        cachedMeshes.erase(modelFilename);
        delete meshes;
    };
    this->meshes = std::shared_ptr<Meshes>(new Meshes, meshesDeleter);

    processNode(*scene->mRootNode, *scene, modelDirectory);

    std::cout << "Successfully loaded model from file: " << modelFilepath << "\n";
    cachedMeshes[modelFilename] = this->meshes;
}

void InstancingGameObjects::processNode(const aiNode &node, const aiScene &scene, const std::string &modelDirectory) {
    // Process node's meshes.
    for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
        const auto mesh = scene.mMeshes[node.mMeshes[i]];
        const auto material = scene.mMaterials[mesh->mMaterialIndex];

        this->meshes->push_back(std::make_unique<InstancingMesh>(*mesh, *material, modelDirectory));
        this->meshes->back()->addModelMatrixAttrib(this->modelMatrixBufferObject);
        this->meshes->back()->addNormalMatrixAttrib(this->normalMatrixBufferObject);
    }

    // Recursively process children nodes.
    for (unsigned int i = 0; i < node.mNumChildren; ++i) {
        processNode(*node.mChildren[i], scene, modelDirectory);
    }
}

InstancingGameObjects::~InstancingGameObjects() {
    glDeleteBuffers(1, &this->normalMatrixBufferObject);
    glDeleteBuffers(1, &this->modelMatrixBufferObject);
}

void InstancingGameObjects::onUpdate(std::chrono::duration<float> updateDuration) {}

void InstancingGameObjects::render(ShaderProgram *shader) {
    constexpr static auto mat3Size_bytes = sizeof(glm::mat3);
    constexpr static auto mat4Size_bytes = sizeof(glm::mat4);

    // Update instance matrix buffer object data
    for (auto idx : this->changedModelIndices) {
        auto modelMatrix = this->models[idx].getModelMatrix();
        glBindBuffer(GL_ARRAY_BUFFER, this->modelMatrixBufferObject);
        glBufferSubData(GL_ARRAY_BUFFER, idx * mat4Size_bytes, mat4Size_bytes,
                        glm::value_ptr(modelMatrix));

        auto normalMatrix = this->models[idx].getNormalMatrix();
        glBindBuffer(GL_ARRAY_BUFFER, this->normalMatrixBufferObject);
        glBufferSubData(GL_ARRAY_BUFFER, idx * mat3Size_bytes, mat3Size_bytes,
                        glm::value_ptr(normalMatrix));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    this->changedModelIndices.clear();

    for (const auto& mesh : *this->meshes) {
        mesh->render(shader, this->models.size());
    }
}

/// ----------------------------------------------------
///           Nested InstancingModel Functions
/// ----------------------------------------------------
InstancingGameObjects::InstancingModel::InstancingModel(InstancingGameObjects &parentGameObject, size_t idx)
    : parentGameObject(parentGameObject), idx(idx) {}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::setPosition(const glm::vec3 &position) {
    this->model.setPosition(position);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::setOrientation(const glm::mat3 &orientation) {
    this->model.setOrientation(orientation);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::setOrientation(const glm::vec3 &orientationX,
                                                                                     const glm::vec3 &orientationY,
                                                                                     const glm::vec3 &orientationZ) {
    this->model.setOrientation(orientationX, orientationY, orientationZ);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::rotate(float angle_rad, const glm::vec3 &axis) {
    this->model.rotate(angle_rad, axis);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::translate(const glm::vec3 &translation) {
    this->model.translate(translation);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::translateInLocalFrame(const glm::vec3 &translation) {
    this->model.translateInLocalFrame(translation);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::setScale(const glm::vec3 &scale) {
    this->model.setScale(scale);
    this->notifyModelChanged();
}

InstancingGameObjects::InstancingModel& InstancingGameObjects::InstancingModel::notifyModelChanged() {
    parentGameObject.changedModelIndices.insert(this->idx);
    return *this;
}

} // namespace gl
