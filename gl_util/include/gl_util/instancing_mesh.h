#pragma once

#include <gl_util/mesh.h>

namespace gl {

///
/// \brief The InstancingMesh class is a proxy class for Mesh that uses
///        instanced rendering to optimize drawing many objects sharing
///        the same vertex data.
///
class InstancingMesh : private Mesh {
public:
    InstancingMesh(const aiMesh &mesh, const aiMaterial &material, const std::string &textureDirectory);
    InstancingMesh& addModelMatrixAttrib(unsigned int modelMatrixBufferObject);
    InstancingMesh& addNormalMatrixAttrib(unsigned int normalMatrixBufferObject);

    void render(ShaderProgram *shader, size_t numInstances);
};

} // namespace gl
