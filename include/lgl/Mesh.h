#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Texture2D.h"

namespace lgl {

class Vertex;
class ShaderProgram;

class Mesh {
private:
    using IndexContainer = std::vector<unsigned int>;

public:
    Mesh(const std::vector<Vertex> &vertices,
         const IndexContainer &indices,
         const std::vector<std::string> &diffuseTextures,
         const std::vector<std::string> &specularTextures);

    void render(ShaderProgram *shaderProgram);

private:
    std::unique_ptr<unsigned int, void(*)(unsigned int *)> vao;
    std::unique_ptr<unsigned int, void(*)(unsigned int *)> vbo;
    std::unique_ptr<unsigned int, void(*)(unsigned int *)> ebo;
    std::vector<Texture2D> diffuseTextures;
    std::vector<Texture2D> specularTextures;
    IndexContainer::size_type numIndices;
};

} // namespace lgl
