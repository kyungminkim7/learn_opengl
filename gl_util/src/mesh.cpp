#include <gl_util/mesh.h>

#include <cstddef>
#include <iostream>

#include <glad/glad.h>

namespace gl {

Mesh::Mesh(std::vector<Vertex> &&vertices,
           std::vector<unsigned int> &&indices,
           std::vector<Texture> &&ambientTextures,
           std::vector<Texture> &&diffuseTextures,
           std::vector<Texture> &&specularTextures) :
    vertices(std::move(vertices)),
    indices(std::move(indices)),
    ambientTextures(std::move(ambientTextures)),
    diffuseTextures(std::move(diffuseTextures)),
    specularTextures(std::move(specularTextures)) {

    // Load vertex data onto GPU.
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    const auto vertexSize = sizeof(decltype(this->vertices)::value_type);
    const auto indexSize = sizeof(decltype(this->indices)::value_type);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(this->vertices.size() * vertexSize),
                 this->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(this->indices.size() * indexSize),
                 this->indices.data(), GL_STATIC_DRAW);

    // Position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize,
                          reinterpret_cast<GLvoid*>(Vertex::offsetOfPosition()));

    // Normal attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize,
                          reinterpret_cast<GLvoid*>(Vertex::offsetOfNormal()));

    // Texture coordinate attribute.
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
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indices.size()),
                   GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
    glBindVertexArray(0);
}

} // namespace gl

