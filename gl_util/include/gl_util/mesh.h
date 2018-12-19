#pragma once

#include <vector>

#include <assimp/material.h>
#include <assimp/mesh.h>

#include <gl_util/shader_program.h>
#include <gl_util/texture.h>

namespace gl {

///
/// \brief Aggregates vertex and index data to load onto the GPU
///        and render onto the screen.
///
class Mesh {
public:    
    ///
    /// \brief Generates a VAO, VBO and EBO for the mesh data,
    ///        loads texture data from the material and
    ///        loads all data onto the GPU.
    /// \param mesh Assimp mesh data to load.
    /// \param material Assimp material data to load for this mesh.
    /// \param textureDirectory Directory path containing all of the
    ///                         textures in this material.
    ///
    Mesh(const aiMesh &mesh, const aiMaterial &material, const std::string &textureDirectory);

    ///
    /// \brief Deletes VAO, VBO and EBO data from the GPU.
    ///
    virtual ~Mesh();

    void render(ShaderProgram *shader);

protected:
    unsigned int getNumIndices() const;
    void bindVao();
    void bindTextures(ShaderProgram *shader);

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int numIndices;

    std::vector<Texture> ambientTextures;
    std::vector<Texture> diffuseTextures;
    std::vector<Texture> specularTextures;
};

inline unsigned int Mesh::getNumIndices() const {return this->numIndices;}

} // namespace gl
