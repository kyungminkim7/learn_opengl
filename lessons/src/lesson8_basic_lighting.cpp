/// Follows "Basic Lighting" lesson.
/// Implements Phong shading for lighting.
#include <iostream>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl_util/camera.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>

std::vector<std::shared_ptr<gl::GameObject>> gameObjects;
std::shared_ptr<gl::Camera> cam;

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>&) {
    return N * sizeof(T);
}

void frameBufferSizeCb(GLFWwindow *window, int width, int height);
void keyCb(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPosCb(GLFWwindow *window, double x, double y);
void scrollCb(GLFWwindow *window, double xOffset, double yOffset);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 8 - Colors");
    if (window == nullptr) return -1;

    if (!gl::initializeGLAD()) return -1;

    // Set up window settings and callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCb);
    glfwSetCursorPosCallback(window, &cursorPosCb);
    glfwSetKeyCallback(window, &keyCb);
    glfwSetScrollCallback(window, &scrollCb);

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);

    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    // Setup camera
    cam = std::make_shared<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                       0.1f, 100.0f);
    cam->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    cam->setLookAtPoint(glm::vec3(0.0f));
    cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));
    gameObjects.push_back(cam);

    std::string dir("../../learn_opengl/lessons/src/");

    gl::ShaderProgram objectShader(dir + "lesson8_object.vert",
                                   dir + "lesson8_object.frag");

    gl::ShaderProgram lampShader(dir + "lesson8_lamp.vert",
                                 dir + "lesson8_lamp.frag");

    constexpr std::size_t numAttributes = 6;
    constexpr auto numFaces = 6;
    constexpr auto numVerticesPerFace = 4;

    // Cube vertex data
    std::array<float, numFaces * numVerticesPerFace * numAttributes> vertices = {
        // position             // normal
        // Front face
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f, // Top left
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f, // Top right
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f, // Bottom right
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f, // Bottom left

        // Left face
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f, // Top left
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f, // Top right
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f, // Bottom left

        // Right face
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f, // Top left
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f, // Top right
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f, // Bottom right
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f, // Bottom left

        // Back face
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f, // Top left
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f, // Top right
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f, // Bottom right
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f, // Bottom left

        // Top face
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f, // Top left
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f, // Top right
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f, // Bottom right
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f, // Bottom left

        // Bottom face
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f, // Top left
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f, // Top right
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f  // Bottom left
    };

    constexpr auto trianglesPerFace = 2;
    constexpr auto verticesPerTriangle = 3;

    // Cube
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

    unsigned int cubeVao, vbo, ebo;
    glGenVertexArrays(1, &cubeVao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(cubeVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Copy vertex & index data into GPU buffer's memory
    glBufferData(GL_ARRAY_BUFFER, arrayDataSize(vertices), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, arrayDataSize(indices), indices.data(), GL_STATIC_DRAW);

    auto stride = numAttributes * sizeof(float);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    // Create game objects
    auto cube = std::make_shared<gl::GameObject>();

    auto lamp = std::make_shared<gl::GameObject>();
    lamp->setScale(glm::vec3(0.2f));
    lamp->setPosition({1.2f, 1.0f, 2.0f});

    gameObjects.push_back(cube);
    gameObjects.push_back(lamp);

    auto lastUpdateTime = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto currentUpdateTime = std::chrono::system_clock::now();
        auto updateDuration = currentUpdateTime - lastUpdateTime;
        lastUpdateTime = currentUpdateTime;

        for (auto& gameObject : gameObjects) {
            gameObject->onUpdate(updateDuration);
        }

        lamp->setPosition({std::sin(glfwGetTime()) * 2.0f,
                          0.0f,
                          1.5f * std::cos(glfwGetTime())});

        // Render game objects
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        objectShader.use();
        objectShader.setUniform("view", cam->getViewMatrix());
        objectShader.setUniform("projection", cam->getProjectionMatrix());
        objectShader.setUniform("viewPosition", cam->getPosition());

        // Set light properties
        objectShader.setUniform("lightPosition", lamp->getPosition());
        objectShader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);

        // Render cube
        objectShader.setUniform("model", cube->getModelMatrix());
        objectShader.setUniform("normal", cube->getNormalMatrix());
        objectShader.setUniform3f("objectColor", 1.0f, 0.5f, 0.31f);

        glBindVertexArray(cubeVao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

        // Render lamp
        lampShader.use();
        lampShader.setUniform("model", lamp->getModelMatrix());
        lampShader.setUniform("view", cam->getViewMatrix());
        lampShader.setUniform("projection", cam->getProjectionMatrix());

        glBindVertexArray(lampVao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVao);
    glDeleteVertexArrays(1, &lampVao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}

void frameBufferSizeCb(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    cam->setAspectRatioWidthToHeight(static_cast<float>(width) / height);
}

void keyCb(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    cam->onKeyInput(window, key, action, mods);
}

void cursorPosCb(GLFWwindow *window, double x, double y) {
    cam->onCursorMoved(window, x, y);
}

void scrollCb(GLFWwindow *window, double xOffset, double yOffset) {
    cam->onScrollInput(window, xOffset, yOffset);
}
