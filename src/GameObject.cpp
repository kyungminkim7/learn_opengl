#include <lgl/GameObject.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <utility>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/vec2.hpp>

#include <lgl/Exception.h>
#include <lgl/Texture2D.h>
#include <lgl/Vertex.h>

namespace {

std::vector<lgl::Texture2D> loadMaterialTextures(aiMaterial *material,
                                                 aiTextureType type) {
    std::vector<lgl::Texture2D> textures;
    textures.reserve(material->GetTextureCount(type));
    for (auto i = 0u; i < material->GetTextureCount(type); ++i) {
        aiString pathname;
        material->GetTexture(type, i, &pathname);
        textures.emplace_back(pathname.C_Str());
    }
    return textures;
}

lgl::Mesh processMesh(const aiMesh *mesh, const aiScene *scene) {
    // Copy vertex data
    std::vector<lgl::Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    for (auto i = 0u; i < mesh->mNumVertices; ++i) {
        vertices.emplace_back(glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                              glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
                              mesh->mTextureCoords[0] ?
                                glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} :
                                glm::vec2(0.0f));
    }

    // Copy index data
    std::vector<unsigned int> indices;
    const auto numIndices = std::accumulate(mesh->mFaces, mesh->mFaces + mesh->mNumFaces, 0u,
                                            [](const auto sum, const auto &face){ return sum + face.mNumIndices; });
    indices.reserve(numIndices);
    std::for_each(mesh->mFaces, mesh->mFaces + mesh->mNumFaces,
                  [&indices](const auto& face){ indices.insert(indices.cend(),
                                                               face.mIndices,
                                                               face.mIndices + face.mNumIndices); });


    // Load textures
    std::vector<lgl::Texture2D> diffuseTextures;
    std::vector<lgl::Texture2D> specularTextures;
    if (mesh->mMaterialIndex >= 0) {
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR);
    }

    return lgl::Mesh(vertices, indices,
                     std::move(diffuseTextures), std::move(specularTextures));
}

} // namespace

namespace lgl {

GameObject::GameObject(const std::string &pathname) {
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(pathname,
                                         aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene ||
            scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) {
        throw LoadError("Failed to load model from: " + pathname);
    }

    this->processNode(scene->mRootNode, scene);
}

void GameObject::processNode(const aiNode *node, const aiScene *scene) {
    this->meshes.reserve(this->meshes.size() + node->mNumMeshes);
    std::transform(node->mMeshes, node->mMeshes + node->mNumMeshes,
                   std::back_inserter(this->meshes),
                   [scene](const auto i){ return processMesh(scene->mMeshes[i], scene); });

    std::for_each(node->mChildren, node->mChildren + node->mNumChildren,
                  [this, scene](const auto child){ this->processNode(child, scene); });
}

void GameObject::onUpdate(Duration duration) {

}

void GameObject::render(ShaderProgram *shaderProgram) {
    std::for_each(this->meshes.begin(), this->meshes.end(),
                  [shaderProgram](auto &m){ m.render(shaderProgram); });
}

} // namespace lgl
