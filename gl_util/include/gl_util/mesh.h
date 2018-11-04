#pragma once

#include <vector>

#include <gl_util/shader_program.h>
#include <gl_util/texture.h>
#include <gl_util/vertex.h>

namespace gl {

///
/// \brief Aggregates vertex and index data to load onto the GPU
///        and render onto the screen.
///
class Mesh
{
public:
    ///
    /// \brief Mesh Generates a VAO, VBO and EBO for the mesh data and
    ///             loads it onto the GPU.
    /// \param vertices Vertex attribute data.
    /// \param indices Index data for the EBO.
    /// \param diffuseTextures Texture data for diffuse lighting.
    /// \param specularTextures Texture data for specular lighting.
    ///
    Mesh(std::vector<Vertex>&& vertices,
         std::vector<unsigned int>&& indices,
         std::vector<Texture>&& diffuseTextures,
         std::vector<Texture>&& specularTextures);

    ///
    /// \brief Deletes VAO, VBO and EBO data from the GPU.
    ///
    ~Mesh();

    void render(ShaderProgram *shader);

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> diffuseTextures;
    std::vector<Texture> specularTextures;
};

} // namespace gl
