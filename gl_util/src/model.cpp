#include <gl_util/model.h>

#include <iostream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <gl_util/gl_exception.h>
#include <gl_util/texture.h>

namespace {

using ModelMeshes = std::vector<std::unique_ptr<gl::Mesh>>;

std::unordered_map<std::string, std::weak_ptr<ModelMeshes>> cachedModelMeshes;

///
/// \brief loadModel Loads and caches mesh and texture data from model file.
///
/// Mesh and texture caches will automatically be cleaned up as the last
/// reference to the returned shared_ptr<Meshes> is destroyed.
///
/// \param modelFilepath Absolute filepath to the model data.
/// \return Shared pointer to the model data (meshes).
/// \exception gl::LoadError Failed to load mesh data from model file.
/// \exception gl::LoadError Failed to load texture image from file.
///
std::shared_ptr<ModelMeshes> loadModel(const std::string &modelFilepath);
void processNode(ModelMeshes *meshes, const aiNode &node, const aiScene &scene, const std::string &modelDirectory);

std::shared_ptr<ModelMeshes> loadModel(const std::string &modelFilepath) {
    auto modelFilenameIndex = modelFilepath.find_last_of('/');
    const auto modelDirectory = modelFilepath.substr(0, modelFilenameIndex);
    const auto modelFilename = modelFilepath.substr(modelFilenameIndex + 1);

    // Check cached meshes to avoid reloading
    auto meshes = cachedModelMeshes[modelFilename].lock();
    if (meshes) return meshes;

    // Load meshes from file
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(modelFilepath,
                                         aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw gl::LoadError(importer.GetErrorString());
    }

    auto meshesDeleter = [modelFilename](auto meshes){
        // Clear cache
        cachedModelMeshes.erase(modelFilename);
        delete meshes;
    };
    meshes = std::shared_ptr<ModelMeshes>(new ModelMeshes, meshesDeleter);

    processNode(meshes.get(), *scene->mRootNode, *scene, modelDirectory);

    std::cout << "Successfully loaded model from file: " << modelFilepath << "\n";
    cachedModelMeshes[modelFilename] = meshes;
    return meshes;
}

void processNode(ModelMeshes *meshes, const aiNode &node, const aiScene &scene, const std::string &modelDirectory) {
    // Process node's meshes.
    for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
        const auto mesh = scene.mMeshes[node.mMeshes[i]];
        const auto material = scene.mMaterials[mesh->mMaterialIndex];
        meshes->push_back(std::make_unique<gl::Mesh>(*mesh, *material, modelDirectory));
    }

    // Recursively process children nodes.
    for (unsigned int i = 0; i < node.mNumChildren; ++i) {
        processNode(meshes, *node.mChildren[i], scene, modelDirectory);
    }
}

} // namespace

namespace gl {

Model::Model() : meshes(std::make_shared<ModelMeshes>()) {}

Model::Model(const std::string& modelFilepath)
    : meshes(loadModel(modelFilepath)) {}

glm::mat4 Model::getModelMatrix() const {
    glm::mat4 modelMatrix(this->orientation);

    for (int i = 0; i < 3; ++i) {
        modelMatrix[3][i] = this->position[i];
    }

    return glm::scale(modelMatrix, this->scale);
}

glm::mat3 Model::getNormalMatrix() const {
    if (!this->normalMatrixIsValid) {
        this->normalMatrix = glm::transpose(glm::inverse(this->getModelMatrix()));
        this->normalMatrixIsValid = true;
    }

    return this->normalMatrix;
}

Model& Model::setPosition(const glm::vec3 &position) {
    this->position = position;

    this->normalMatrixIsValid = false;
    return *this;
}

Model& Model::setOrientation(const glm::mat3 &orientation) {
    this->orientation = orientation;

    this->normalMatrixIsValid = false;
    return *this;
}

Model& Model::setOrientation(const glm::vec3 &orientationX,
                             const glm::vec3 &orientationY,
                             const glm::vec3 &orientationZ) {
    this->orientation[0] = orientationX;
    this->orientation[1] = orientationY;
    this->orientation[2] = orientationZ;

    this->normalMatrixIsValid = false;
    return *this;
}

Model& Model::rotate(float angle_rad, const glm::vec3 &axis) {
    auto rotation = static_cast<glm::mat3>(glm::rotate(glm::mat4(1.0f), angle_rad, axis));
    this->orientation = rotation * this->orientation;

    this->normalMatrixIsValid = false;
    return *this;
}

Model& Model::translate(const glm::vec3 &translation) {
    this->position += translation;

    this->normalMatrixIsValid = false;
    return *this;
}

Model& Model::translateInLocalFrame(const glm::vec3 &translation) {
    this->position += (this->orientation * translation);

    this->normalMatrixIsValid = false;
    return *this;
}

Model& Model::setScale(const glm::vec3 &scale) {
    this->scale = scale;

    this->normalMatrixIsValid = false;
    return *this;
}

void Model::render(ShaderProgram *shader) {
    shader->setUniform("model", this->getModelMatrix())
            .setUniform("normal", this->getNormalMatrix());

    for (const auto& mesh : *this->meshes) {
        mesh->render(shader);
    }
}

} // namespace gl
