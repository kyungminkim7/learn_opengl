/// Follows "Assimp", "Mesh", and "Model" lessons.
/// Imports a model using the Assimp library into a Model object.
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <gl_util/camera.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>

std::shared_ptr<gl::Camera> cam;

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>&) {
    return N * sizeof(T);
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 13 - Model");
    if (window == nullptr) return -1;

    if (!gl::initializeGLAD()) return -1;

    // Set up window settings and callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, &cursorPositionCallback);
    glfwSetKeyCallback(window, &keyCallback);
    glfwSetScrollCallback(window, &scrollCallback);

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);

    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    std::vector<std::shared_ptr<gl::GameObject>> gameObjects;

    // Setup camera
    cam = std::make_shared<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                       0.1f, 100.0f);
    cam->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    cam->setLookAtPoint(glm::vec3(0.0f));
    cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));
    gameObjects.push_back(cam);

    std::string baseDir("../../learn_opengl/lessons/");
    auto srcDir = baseDir + "src/";

    gl::ShaderProgram objectShader(srcDir + "lesson13_object.vert",
                                   srcDir + "lesson13_object.frag");

    gl::ShaderProgram lampShader(srcDir + "lesson13_lamp.vert",
                                 srcDir + "lesson13_lamp.frag");

    constexpr std::size_t numAttributes = 8;
    constexpr auto numFaces = 6;
    constexpr auto numVerticesPerFace = 4;

    // Cube vertex data
    std::array<float, numFaces * numVerticesPerFace * numAttributes> vertices = {
        // position             // normal               // tex coord
        // Front face
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f, 1.0f, // Top left
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // Top right
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // Bottom left

        // Left face
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // Top left
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // Top right
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // Bottom left

        // Right face
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // Top left
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // Top right
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // Bottom right
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // Bottom left

        // Back face
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // Top left
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // Top right
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // Bottom right
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // Bottom left

        // Top face
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // Top left
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f, // Top right
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // Bottom right
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f, 0.0f, // Bottom left

        // Bottom face
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // Top left
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f, // Top right
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f, 0.0f  // Bottom left
    };

    constexpr auto trianglesPerFace = 2;
    constexpr auto verticesPerTriangle = 3;

    std::array<unsigned int, numFaces * trianglesPerFace * verticesPerTriangle> indices = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Copy vertex & index data into GPU buffer's memory
    glBufferData(GL_ARRAY_BUFFER, arrayDataSize(vertices), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, arrayDataSize(indices), indices.data(), GL_STATIC_DRAW);

    auto stride = numAttributes * sizeof(float);

    // Construct lamp
    unsigned int lampVao;
    glGenVertexArrays(1, &lampVao);
    glBindVertexArray(lampVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); // can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind after VAO since VAO records all ebo binds/unbinds

    // Create nanosuit game object
    auto nanosuit = std::make_shared<gl::GameObject>(baseDir + "models/nanosuit/nanosuit.obj");
    gameObjects.push_back(nanosuit);
    nanosuit->setPosition({0.0f, -3.0f, 0.0f});
    nanosuit->setScale(glm::vec3(0.3f));

    auto nanosuit2 = std::make_shared<gl::GameObject>(baseDir + "models/nanosuit/nanosuit.obj");
    gameObjects.push_back(nanosuit2);
    nanosuit2->setPosition({2.0f, -3.0f, 0.0f});
    nanosuit2->setScale(glm::vec3(0.3f));
    nanosuit2->rotate(glm::radians(45.0f), nanosuit2->getOrientationY());

    // Create lamps
    std::vector<glm::vec3> lampPositions {
        {0.7f, 0.2f, 2.0f},
        {2.3f, -3.3f, -4.0f},
        {-4.0f, 2.0f, -12.0f},
        {0.0f, 0.0f, -3.0f}
    };

    std::vector<std::shared_ptr<gl::GameObject>> lamps(lampPositions.size());
    for (size_t i = 0; i < lamps.size(); ++i) {
        auto lamp = std::make_shared<gl::GameObject>();
        lamp->setScale(glm::vec3(0.2f));
        lamp->setPosition(lampPositions[i]);

        lamps[i] = lamp;
        gameObjects.push_back(std::move(lamp));
    }

    auto lastUpdateTime = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto currentUpdateTime = std::chrono::system_clock::now();
        auto updateDuration = currentUpdateTime - lastUpdateTime;
        lastUpdateTime = currentUpdateTime;

        for (auto& gameObject : gameObjects) {
            gameObject->onUpdate(updateDuration);
        }

        // Render game objects
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        objectShader.use();

        // Set directional light
        objectShader.setUniform("directionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f))
                .setUniform("directionalLight.lighting.ambient", glm::vec3(0.2f))
                .setUniform("directionalLight.lighting.diffuse", glm::vec3(0.5f))
                .setUniform("directionalLight.lighting.specular", glm::vec3(1.0f));

        // Set point lights
        for (size_t i = 0; i < lamps.size(); ++i) {
            std::stringstream attribIndex;
            attribIndex << "pointLights[" << i << "].";
            objectShader.setUniform(attribIndex.str() + "position", lamps[i]->getPosition())
                    .setUniform(attribIndex.str() + "lighting.ambient", glm::vec3(0.2f))
                    .setUniform(attribIndex.str() + "lighting.diffuse", glm::vec3(0.5f))
                    .setUniform(attribIndex.str() + "lighting.specular", glm::vec3(1.0f))
                    .setUniform(attribIndex.str() + "attenuation.constant", 1.0f)
                    .setUniform(attribIndex.str() + "attenuation.linear", 0.09f)
                    .setUniform(attribIndex.str() + "attenuation.quadratic", 0.032f);
        }

        // Set spot light
        objectShader.setUniform("spotLight.position", cam->getPosition())
                .setUniform("spotLight.direction", cam->getLookAtDirection())
                .setUniform("spotLight.innerCutOff", std::cos(glm::radians(12.5f)))
                .setUniform("spotLight.outerCutoff", std::cos(glm::radians(17.5f)))
                .setUniform("spotLight.lighting.ambient", glm::vec3(0.2f))
                .setUniform("spotLight.lighting.diffuse", glm::vec3(0.5f))
                .setUniform("spotLight.lighting.specular", glm::vec3(1.0f))
                .setUniform("spotLight.attenuation.constant", 1.0f)
                .setUniform("spotLight.attenuation.linear", 0.09f)
                .setUniform("spotLight.attenuation.quadratic", 0.032f);

        // Set camera properties
        objectShader.setUniform("viewPosition", cam->getPosition())
                .setUniform("view", cam->getViewMatrix())
                .setUniform("projection", cam->getProjectionMatrix());

        // Render game objects
        objectShader.setUniform("material.shininess", 32.0f);
        for (auto& gameObject : gameObjects) {
            gameObject->render(&objectShader);
        }

        // Render lamps
        lampShader.use()
                .setUniform("view", cam->getViewMatrix())
                .setUniform("projection", cam->getProjectionMatrix());
        glBindVertexArray(lampVao);

        for (const auto& lamp : lamps) {
            lampShader.setUniform("model", lamp->getModelMatrix());
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &lampVao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

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
