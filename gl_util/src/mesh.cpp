#include <gl_util/mesh.h>

#include <cstddef>
#include <iostream>

#include <glad/glad.h>

#include <glm/vec2.hpp>

namespace {

std::vector<gl::Texture> loadTexturesFromMaterial(const aiMaterial &material, aiTextureType type, const std::string &modelDirectory) {
    std::vector<gl::Texture> textures;
    textures.reserve(material.GetTextureCount(type));

    for (unsigned int i = 0; i < material.GetTextureCount(type); ++i) {
        aiString imageFilename;
        material.GetTexture(type, i, &imageFilename);
        textures.emplace_back(modelDirectory + "/" + imageFilename.C_Str());
    }

    return textures;
}

} // namespace

namespace gl {

Mesh::Mesh(const aiMesh &mesh, const aiMaterial &material, const std::string &textureDirectory) {
    // Load texture coordinates into appropriate data structure.
    std::vector<glm::vec2> textureCoords;
    textureCoords.reserve(mesh.mNumVertices);
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
        textureCoords.emplace_back(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y);
    }

    // Process indices.
    std::vector<unsigned int> indices;
    indices.reserve(mesh.mNumVertices);
    for (unsigned int i = 0; i < mesh.mNumFaces; ++i) {
        const auto& face = mesh.mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.emplace_back(face.mIndices[j]);
        }
    }
    this->numIndices = indices.size();

    constexpr static auto positionSize_bytes = sizeof(aiVector3D);
    constexpr static auto normalSize_bytes = sizeof(aiVector3D);
    constexpr static auto textureCoordSize_bytes = sizeof(decltype(textureCoords)::value_type);
    constexpr static auto indexSize_bytes = sizeof(decltype(indices)::value_type);

    const auto positionArraySize_bytes = mesh.mNumVertices * positionSize_bytes;
    const auto normalArraySize_bytes = mesh.mNumVertices * normalSize_bytes;
    const auto textureCoordArraySize_bytes = textureCoords.size() * textureCoordSize_bytes;

    // Load vertex data onto GPU.
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 positionArraySize_bytes + normalArraySize_bytes + textureCoordArraySize_bytes,
                 nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positionArraySize_bytes, mesh.mVertices);
    glBufferSubData(GL_ARRAY_BUFFER, positionArraySize_bytes, normalArraySize_bytes, mesh.mNormals);
    glBufferSubData(GL_ARRAY_BUFFER, positionArraySize_bytes + normalArraySize_bytes,
                    textureCoordArraySize_bytes, textureCoords.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * indexSize_bytes,
                 indices.data(), GL_STATIC_DRAW);

    // Define position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, positionSize_bytes,
                          reinterpret_cast<GLvoid*>(0));

    // Define normal attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, normalSize_bytes,
                          reinterpret_cast<GLvoid*>(positionArraySize_bytes));

    // Define texture coordinate attribute.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, textureCoordSize_bytes,
                          reinterpret_cast<GLvoid*>(positionArraySize_bytes + normalArraySize_bytes));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Load textures.
    this->ambientTextures = loadTexturesFromMaterial(material, aiTextureType_AMBIENT, textureDirectory);
    this->diffuseTextures = loadTexturesFromMaterial(material, aiTextureType_DIFFUSE, textureDirectory);
    this->specularTextures = loadTexturesFromMaterial(material, aiTextureType_SPECULAR, textureDirectory);
}

Mesh::Mesh(const std::vector<float> &positions,
           const std::vector<float> &normals,
           const std::vector<float> &textureCoords,
           const std::vector<unsigned int> &indices,
           const std::string &textureFilepath) {
    constexpr static auto positionSize_bytes = 3 * sizeof(float);
    constexpr static auto normalSize_bytes = 3 * sizeof(float);
    constexpr static auto textureCoordSize_bytes = 2 * sizeof(float);
    constexpr static auto indexSize_bytes = sizeof(unsigned int);

    const auto positionArraySize_bytes = positions.size() * sizeof(float);
    const auto normalArraySize_bytes = normals.size() * sizeof(float);
    const auto textureCoordArraySize_bytes = textureCoords.size() * sizeof(float);

    this->numIndices = indices.size();

    // Load vertex data onto GPU
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 positionArraySize_bytes + normalArraySize_bytes + textureCoordArraySize_bytes,
                 nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, positionArraySize_bytes, positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, positionArraySize_bytes, normalArraySize_bytes, normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, positionArraySize_bytes + normalArraySize_bytes,
                    textureCoordArraySize_bytes, textureCoords.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * indexSize_bytes,
                 indices.data(), GL_STATIC_DRAW);


    // Define position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, positionSize_bytes,
                          reinterpret_cast<GLvoid*>(0));

    // Define normal attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, normalSize_bytes,
                          reinterpret_cast<GLvoid*>(positionArraySize_bytes));

    // Define texture coordinate attribute.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, textureCoordSize_bytes,
                          reinterpret_cast<GLvoid*>(positionArraySize_bytes + normalArraySize_bytes));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (!textureFilepath.empty()) {
        gl::Texture texture(textureFilepath);
        this->ambientTextures.push_back(texture);
        this->diffuseTextures.push_back(texture);
    }
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

void Mesh::render(ShaderProgram *shader) {
    this->bindTextures(shader);

    // Draw mesh
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->numIndices),
                   GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
    glBindVertexArray(0);
}

void Mesh::bindVao() {
    glBindVertexArray(this->vao);
}

void Mesh::bindTextures(ShaderProgram *shader) {
    // Bind textures
    int textureUnit = 0;

    for (size_t i = 0; i < this->ambientTextures.size(); ++i, ++textureUnit) {
        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit));
        shader->setUniform("material.ambientTexture" + std::to_string(i), textureUnit);
        this->ambientTextures[i].bind();
    }

    for (size_t i = 0; i < this->diffuseTextures.size(); ++i, ++textureUnit) {
        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit));
        shader->setUniform("material.diffuseTexture" + std::to_string(i), textureUnit);
        this->diffuseTextures[i].bind();
    }

    for (size_t i = 0; i < this->specularTextures.size(); ++i, ++textureUnit) {
        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit));
        shader->setUniform("material.specularTexture" + std::to_string(i), textureUnit);
        this->specularTextures[i].bind();
    }

    glActiveTexture(GL_TEXTURE0);
}

} // namespace gl

