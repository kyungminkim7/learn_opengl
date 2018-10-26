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
    Mesh(std::vector<Vertex>&& vertices,
         std::vector<unsigned int>&& indices,
         std::vector<Texture>&& diffuseTextures,
         std::vector<Texture>&& specularTextures);

    const std::vector<Texture>& getDiffuseTextures() const;
    const std::vector<Texture>& getSpecularTextures() const;

    void render(ShaderProgram *shader) const;

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
