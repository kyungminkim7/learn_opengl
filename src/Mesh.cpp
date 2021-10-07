#include <lgl/Mesh.h>

#include <utility>

#include <glad/glad.h>

#include <lgl/Vertex.h>
#include <lgl/ShaderProgram.h>

namespace {

void deleteVertexArray(unsigned int *vertexArray) {
    glDeleteVertexArrays(1, vertexArray);
    delete vertexArray;
}

void deleteBuffer(unsigned int *buffer) {
    glDeleteBuffers(1, buffer);
    delete buffer;
}

} // namespace

namespace lgl {

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const IndexContainer &indices,
           const std::vector<Texture2D> diffuseTextures,
           const std::vector<Texture2D> specularTextures) :
    vao(new unsigned int, deleteVertexArray),
    vbo(new unsigned int, deleteBuffer),
    ebo(new unsigned int, deleteBuffer),
    diffuseTextures(std::move(diffuseTextures)),
    specularTextures(std::move(specularTextures)),
    numIndices(indices.size()) {

    glGenVertexArrays(1, this->vao.get());
    glGenBuffers(1, this->vbo.get());
    glGenBuffers(1, this->ebo.get());

    // Copy data into GPU
    glBindVertexArray(*this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, *this->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(IndexContainer::value_type),
                 indices.data(), GL_STATIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(0));

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, normal)));

    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, textureCoordinates)));
}

void Mesh::render(ShaderProgram *shaderProgram) {
    auto textureUnit = 0;

    for (auto i = 0u; i < this->diffuseTextures.size(); ++i, ++textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        shaderProgram->setUniform("material.diffuseTexture" + std::to_string(i), textureUnit);
        this->diffuseTextures[i].bind();
    }

    for (auto i = 0u; i < this->specularTextures.size(); ++i, ++textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        shaderProgram->setUniform("material.specularTexture" + std::to_string(i), textureUnit);
        this->specularTextures[i].bind();
    }

    glBindVertexArray(*this->vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

} // namespace lgl
