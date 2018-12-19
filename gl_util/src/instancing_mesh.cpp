#include <gl_util/instancing_mesh.h>

#include <glad/glad.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace gl {

InstancingMesh::InstancingMesh(const aiMesh &mesh, const aiMaterial &material, const std::string &textureDirectory)
    : Mesh(mesh, material, textureDirectory){}

InstancingMesh& InstancingMesh::addModelMatrixAttrib(unsigned int modelMatrixBufferObject) {
    this->bindVao();
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferObject);

    constexpr static auto vectorSize = sizeof(glm::vec4);
    constexpr static auto startingAttribIdx = 3u;
    constexpr static auto numAttribElements = 4;

    auto ptrOffset = 0u;
    for (auto attribIdx = startingAttribIdx; attribIdx < startingAttribIdx + numAttribElements;
         ++attribIdx, ++ptrOffset) {
        glEnableVertexAttribArray(attribIdx);
        glVertexAttribPointer(attribIdx, numAttribElements, GL_FLOAT, GL_FALSE,
                              numAttribElements * vectorSize,
                              reinterpret_cast<GLvoid*>(ptrOffset * vectorSize));
        glVertexAttribDivisor(attribIdx, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return *this;
}

InstancingMesh& InstancingMesh::addNormalMatrixAttrib(unsigned int normalMatrixBufferObject) {
    this->bindVao();
    glBindBuffer(GL_ARRAY_BUFFER, normalMatrixBufferObject);

    constexpr static auto vectorSize = sizeof(glm::vec3);
    constexpr static auto startingAttribIdx = 7u;
    constexpr static auto numAttribElements = 3;

    auto ptrOffset = 0u;
    for (auto attribIdx = startingAttribIdx; attribIdx < startingAttribIdx + numAttribElements;
         ++attribIdx, ++ptrOffset) {
        glEnableVertexAttribArray(attribIdx);
        glVertexAttribPointer(attribIdx, numAttribElements, GL_FLOAT, GL_FALSE,
                              numAttribElements * vectorSize,
                              reinterpret_cast<GLvoid*>(ptrOffset * vectorSize));
        glVertexAttribDivisor(attribIdx, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return *this;
}

void InstancingMesh::render(ShaderProgram *shader, size_t numInstances) {
    this->bindTextures(shader);

    this->bindVao();
    glDrawElementsInstanced(GL_TRIANGLES, this->getNumIndices(),
                            GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0),
                            numInstances);
    glBindVertexArray(0);
}

} // namespace gl
