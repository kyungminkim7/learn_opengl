/// Follows "Parallax Mapping" lesson.
/// Adds realistic depth/height offset using parallax mapping.

#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <tuple>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <stb_image.h>

#include <gl_util/camera.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/point_light.h>
#include <gl_util/shader_program.h>
#include <gl_util/uniform_buffer.h>

const static std::vector<float> cubeVertexPositions {
    // back face
    -1.0f, -1.0f, -1.0f, // bottom-left
     1.0f,  1.0f, -1.0f, // top-right
     1.0f, -1.0f, -1.0f, // bottom-right
    -1.0f,  1.0f, -1.0f, // top-left
    // front face
    -1.0f, -1.0f,  1.0f, // bottom-left
     1.0f, -1.0f,  1.0f, // bottom-right
     1.0f,  1.0f,  1.0f, // top-right
    -1.0f,  1.0f,  1.0f, // top-left
    // left face
    -1.0f,  1.0f,  1.0f, // top-right
    -1.0f,  1.0f, -1.0f, // top-left
    -1.0f, -1.0f, -1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, // bottom-right
    // right face
     1.0f,  1.0f,  1.0f, // top-left
     1.0f, -1.0f, -1.0f, // bottom-right
     1.0f,  1.0f, -1.0f, // top-right
     1.0f, -1.0f,  1.0f, // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f, // top-right
     1.0f, -1.0f, -1.0f, // top-left
     1.0f, -1.0f,  1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, // bottom-right
    // top face
    -1.0f,  1.0f, -1.0f,  // top-left
     1.0f,  1.0f , 1.0f,  // bottom-right
     1.0f,  1.0f, -1.0f,  // top-right
    -1.0f,  1.0f,  1.0f,  // bottom-left
};

const static std::vector<float> cubeNormals {
    // back face
    0.0f,  0.0f, -1.0, // bottom-left
    0.0f,  0.0f, -1.0f, // top-right
    0.0f,  0.0f, -1.0f, // bottom-right
    0.0f,  0.0f, -1.0f, // top-left
    // front face
    0.0f,  0.0f,  1.0f, // bottom-left
    0.0f,  0.0f,  1.0f, // bottom-right
    0.0f,  0.0f,  1.0f, // top-right
    0.0f,  0.0f,  1.0f, // top-left
    // left face
    -1.0f,  0.0f,  0.0f, // top-right
    -1.0f,  0.0f,  0.0f, // top-left
    -1.0f,  0.0f,  0.0f, // bottom-left
    -1.0f,  0.0f,  0.0f, // bottom-right
    // right face
     1.0f,  0.0f,  0.0f, // top-left
     1.0f,  0.0f,  0.0f, // bottom-right
     1.0f,  0.0f,  0.0f, // top-right
     1.0f,  0.0f,  0.0f, // bottom-left
    // bottom face
     0.0f, -1.0f,  0.0f, // top-right
     0.0f, -1.0f,  0.0f, // top-left
     0.0f, -1.0f,  0.0f, // bottom-left
     0.0f, -1.0f,  0.0f, // bottom-right
    // top face
     0.0f,  1.0f,  0.0f, // top-left
     0.0f,  1.0f,  0.0f, // bottom-right
     0.0f,  1.0f,  0.0f, // top-right
     0.0f,  1.0f,  0.0f  // bottom-left
};

const static std::vector<float> cubeTextureCoords {
    // back face
    0.0f, 0.0f, // bottom-left
    1.0f, 1.0f, // top-right
    1.0f, 0.0f, // bottom-right
    0.0f, 1.0f, // top-left
    // front face
    0.0f, 0.0f, // bottom-left
    1.0f, 0.0f, // bottom-right
    1.0f, 1.0f, // top-right
    0.0f, 1.0f, // top-left
    // left face
    1.0f, 0.0f, // top-right
    1.0f, 1.0f, // top-left
    0.0f, 1.0f, // bottom-left
    0.0f, 0.0f, // bottom-right
    // right face
    1.0f, 0.0f, // top-left
    0.0f, 1.0f, // bottom-right
    1.0f, 1.0f, // top-right
    0.0f, 0.0f, // bottom-left
    // bottom face
    0.0f, 1.0f, // top-right
    1.0f, 1.0f, // top-left
    1.0f, 0.0f, // bottom-left
    0.0f, 0.0f, // bottom-right
    // top face
    0.0f, 1.0f, // top-left
    1.0f, 0.0f, // bottom-right
    1.0f, 1.0f, // top-right
    0.0f, 0.0f  // bottom-left
};

const static std::vector<unsigned int> cubeIndices {
    // back face
    0, 1, 2,
    1, 0, 3,
    // front face
    4, 5, 6,
    6, 7, 4,
    // left face
    8, 9, 10,
    10, 11, 8,
    // right face
    12, 13, 14,
    13, 12, 15,
    // bottom face
    16, 17, 18,
    18, 19, 16,
    // top face
    20, 21, 22,
    21, 20, 23
};

static std::unique_ptr<gl::Camera> cam;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
unsigned int loadTexture(const std::string &imageFilePath);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 28 - Parallax Mapping");
    if (window == nullptr) return -1;

    if (!gl::initializeGLAD()) return -1;

    // Set up window settings and callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, &cursorPositionCallback);
    glfwSetKeyCallback(window, &keyCallback);
    glfwSetScrollCallback(window, &scrollCallback);

    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);

    {
        std::string baseDir("../../learn_opengl/lessons/");
        auto srcDir = baseDir + "src/";

        gl::ShaderProgram defaultShader(srcDir + "lesson28_default.vert",
                                        srcDir + "lesson28_default.frag");

        constexpr auto mat4Size_bytes = sizeof(glm::mat4);
        gl::UniformBuffer matricesUbo(2 * mat4Size_bytes);
        defaultShader.setUniformBlockBinding("Matrices", matricesUbo.getBindingPoint());

        // Setup camera
        cam = std::make_unique<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                           0.1f, 1000.0f);
        cam->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
        cam->setLookAtPoint(glm::vec3(0.0f));
        cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

        // Create quad
        std::vector<glm::vec3> vertexPositions {
            {-1.0f, 1.0f, 0.0f},
            {-1.0f, -1.0f, 0.0f},
            {1.0f, -1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f}
        };

        std::vector<glm::vec2> textureCoords {
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f}
        };

        // Calculate quad tangent and bitangent vectors
        std::vector<glm::vec3> tangents(2), bitangents(2);
        auto calcTB = [&vertexPositions, &textureCoords](const std::vector<size_t> &indices) {
            std::vector<glm::vec3> edges {
                vertexPositions[indices[1]] - vertexPositions[indices[0]],
                vertexPositions[indices[2]] - vertexPositions[indices[0]]
            };

            std::vector<glm::vec2> deltaUV {
                textureCoords[indices[1]] - textureCoords[indices[0]],
                textureCoords[indices[2]] - textureCoords[indices[0]]
            };

            auto f = 1.0f / (deltaUV[0].x * deltaUV[1].y - deltaUV[1].x * deltaUV[0].y);

            glm::vec3 tangent(
                f * (deltaUV[1].y * edges[0].x - deltaUV[0].y * edges[1].x),
                f * (deltaUV[1].y * edges[0].y - deltaUV[0].y * edges[1].y),
                f * (deltaUV[1].y * edges[0].z - deltaUV[0].y * edges[1].z)
            );

            glm::vec3 bitangent(
                f * (-deltaUV[1].x * edges[0].x + deltaUV[0].x * edges[1].x),
                f * (-deltaUV[1].x * edges[0].y + deltaUV[0].x * edges[1].y),
                f * (-deltaUV[1].x * edges[0].z + deltaUV[0].x * edges[1].z)
            );

            return std::make_tuple(glm::normalize(tangent),
                                   glm::normalize(bitangent));
        };

        std::tie(tangents[0], bitangents[0]) = calcTB({0, 1, 2});
        std::tie(tangents[1], bitangents[1]) = calcTB({0, 2, 3});

        // Create quad vertices and generate vao and vbo
        glm::vec3 normal(0.0f, 0.0f, 1.0f);
        std::vector<float> quadVertices;
        auto createVertex = [&quadVertices, &vertexPositions, &textureCoords, &normal,
                &tangents, &bitangents](auto index, auto btIndex) {
            std::copy(glm::value_ptr(vertexPositions[index]), glm::value_ptr(vertexPositions[index]) + 3, std::back_inserter(quadVertices));
            std::copy(glm::value_ptr(normal), glm::value_ptr(normal) + 3, std::back_inserter(quadVertices));
            std::copy(glm::value_ptr(textureCoords[index]), glm::value_ptr(textureCoords[index]) + 2, std::back_inserter(quadVertices));
            std::copy(glm::value_ptr(tangents[btIndex]), glm::value_ptr(tangents[btIndex]) + 3, std::back_inserter(quadVertices));
            std::copy(glm::value_ptr(bitangents[btIndex]), glm::value_ptr(bitangents[btIndex]) + 3, std::back_inserter(quadVertices));
        };

        createVertex(0, 0);
        createVertex(1, 0);
        createVertex(2, 0);
        createVertex(0, 1);
        createVertex(2, 1);
        createVertex(3, 1);

        unsigned int quadVao, quadVbo;
        glGenVertexArrays(1, &quadVao);
        glGenBuffers(1, &quadVbo);
        glBindVertexArray(quadVao);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);

        constexpr auto stride = 14 * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(11 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        auto diffuseMap = loadTexture(baseDir + "images/wood.png");
        auto normalMap = loadTexture(baseDir + "images/toy_box_normal.png");
        auto depthMap = loadTexture(baseDir + "images/toy_box_disp.png");
        gl::Model wallModel;

        glm::vec3 lightPosition(0.5f, 0.5f, 0.3f);

        auto lastUpdateTime = std::chrono::system_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto currentUpdateTime = std::chrono::system_clock::now();
            auto updateDuration = currentUpdateTime - lastUpdateTime;
            lastUpdateTime = currentUpdateTime;

            // Update
            cam->onUpdate(updateDuration);

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            matricesUbo.bufferSubData(0, mat4Size_bytes, glm::value_ptr(cam->getViewMatrix()))
                    .bufferSubData(mat4Size_bytes, mat4Size_bytes, glm::value_ptr(cam->getProjectionMatrix()));

            defaultShader.use();
            defaultShader.setUniform("viewPosition", cam->getPosition())
                    .setUniform("lightPosition", lightPosition)
                    .setUniform("light.lighting.ambient", glm::vec3(0.1f))
                    .setUniform("light.lighting.diffuse", glm::vec3(1.0f))
                    .setUniform("light.lighting.specular", glm::vec3(0.2f));

            // Render wall
            defaultShader.setUniform("material.diffuseTexture0", 0)
                    .setUniform("material.normalMap", 1)
                    .setUniform("material.depthMap", 2)
                    .setUniform("material.heightScale", 0.2f)
                    .setUniform("material.shininess", 32.0f);
            wallModel.render(&defaultShader);
            glBindVertexArray(quadVao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalMap);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Render light
            auto lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, lightPosition);
            lightModel = glm::scale(lightModel, glm::vec3(0.1f));
            defaultShader.setUniform("model", lightModel);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteTextures(1, &depthMap);
        glDeleteTextures(1, &normalMap);
        glDeleteTextures(1, &diffuseMap);
        glDeleteBuffers(1, &quadVbo);
        glDeleteVertexArrays(1, &quadVao);
    }

    glfwTerminate();
    return 0;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    cam->setAspectRatioWidthToHeight(static_cast<float>(width) / height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    cam->onKeyInput(window, key, action, mods);
}

void cursorPositionCallback(GLFWwindow *window, double x, double y) {
    cam->onCursorMoved(window, x, y);
}

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    cam->onScrollInput(window, xOffset, yOffset);
}

unsigned int loadTexture(const std::string &imageFilePath) {
    unsigned int textureId;
    glGenTextures(1, &textureId);

    int width, height, numChannels;
    auto data = stbi_load(imageFilePath.c_str(), &width, &height, &numChannels, 0);

    if (!data) {
        std::cerr << "Failed to load texture at: " << imageFilePath << "\n";
        stbi_image_free(data);
        return textureId;
    }

    GLenum format;
    switch (numChannels) {
    case 1:
        format = GL_RED;
        break;

    case 3:
        format = GL_RGB;
        break;

    case 4:
        format = GL_RGBA;
        break;

    default:
        format = GL_RGB;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D,
                 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureId;
}
