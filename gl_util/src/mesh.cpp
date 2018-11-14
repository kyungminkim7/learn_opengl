#include <gl_util/mesh.h>

#include <cstddef>
#include <iostream>

#include <glad/glad.h>

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
    // Process vertices.
    std::vector<Vertex> vertices;
    vertices.reserve(mesh.mNumVertices);
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
        vertices.emplace_back(glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z),
                              glm::vec3(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z),
                              glm::vec2(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y));
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

    // Process material.
    this->ambientTextures = loadTexturesFromMaterial(material, aiTextureType_AMBIENT, textureDirectory);
    this->diffuseTextures = loadTexturesFromMaterial(material, aiTextureType_DIFFUSE, textureDirectory);
    this->specularTextures = loadTexturesFromMaterial(material, aiTextureType_SPECULAR, textureDirectory);

    const auto vertexSize = sizeof(decltype(vertices)::value_type);
    const auto indexSize = sizeof(decltype(indices)::value_type);

    // Load vertex data onto GPU.
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * vertexSize),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * indexSize),
                 indices.data(), GL_STATIC_DRAW);

    // Define position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize,
                          reinterpret_cast<GLvoid*>(Vertex::offsetOfPosition()));

    // Define normal attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize,
                          reinterpret_cast<GLvoid*>(Vertex::offsetOfNormal()));

    // Define texture coordinate attribute.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          reinterpret_cast<GLvoid*>(Vertex::offsetOfTextureCoordinate()));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

void Mesh::render(ShaderProgram *shader) {
    // Bind textures
    int textureUnit = 0;

    for (size_t i =0; i < this->ambientTextures.size(); ++i, ++textureUnit) {
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

    // Draw mesh
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->numIndices),
                   GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
    glBindVertexArray(0);
}

} // namespace gl

