/// Follows "Depth Testing" lesson.
/// Represents linearized depth values in black and white.
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

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
unsigned int loadTexture(const std::string& imageFilePath);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 14 - Depth Testing");
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

    gl::ShaderProgram shader(srcDir + "lesson14_depth_testing.vert",
                                   srcDir + "lesson14_depth_testing.frag");

    // Construct cube
    std::vector<float> cubeVertices {
        // position             // tex coord
        // Front face
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // Top left
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // Top right
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // Bottom left

        // Left face
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // Top left
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // Top right
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // Bottom left

        // Right face
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // Top left
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // Top right
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // Bottom right
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // Bottom left

        // Back face
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // Top left
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // Top right
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // Bottom right
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // Bottom left

        // Top face
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // Top left
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // Top right
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // Bottom right
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, // Bottom left

        // Bottom face
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, // Top left
         0.5f, -0.5f,  0.5f,    1.0f, 1.0f, // Top right
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f  // Bottom left
    };

    std::vector<unsigned int> cubeIndices {
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

    auto stride = 5 * sizeof(float);

    unsigned int cubeVao, cubeVbo, cubeEbo;
    glGenVertexArrays(1, &cubeVao);
    glBindVertexArray(cubeVao);

    glGenBuffers(1, &cubeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);

    glGenBuffers(1, &cubeEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);

    // Copy vertex & index data into GPU buffer's memory
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof (decltype (cubeVertices)::value_type),
                 cubeVertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof (decltype (cubeIndices)::value_type),
                 cubeIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); // can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind after VAO since VAO records all ebo binds/unbinds

    gl::GameObject cube1, cube2;
    cube1.setPosition({-1.0f, 0.0f, -1.0f});
    cube2.setPosition({2.0f, 0.0f, 0.0f});

    auto cubeTexture = loadTexture(baseDir + "images/marble.jpg");

    // Construct plane
    std::vector<float> planeVertices {
        // positions          // texture Coords
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    std::vector<unsigned int> planeIndices {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int planeVao, planeVbo, planeEbo;
    glGenVertexArrays(1, &planeVao);
    glBindVertexArray(planeVao);

    glGenBuffers(1, &planeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, planeVbo);

    glGenBuffers(1, &planeEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEbo);

    glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof (decltype (planeVertices)::value_type),
                 planeVertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices.size() * sizeof(decltype (planeIndices)::value_type),
                 planeIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<const GLvoid*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<const GLvoid*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    gl::GameObject plane;
    auto planeTexture = loadTexture(baseDir + "images/metal.png");

    shader.use();
    shader.setUniform("texture0", 0);

    auto lastUpdateTime = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto currentUpdateTime = std::chrono::system_clock::now();
        auto updateDuration = currentUpdateTime - lastUpdateTime;
        lastUpdateTime = currentUpdateTime;

        for (auto& gameObject : gameObjects) {
            gameObject->onUpdate(updateDuration);
        }

        // Render game objects
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Set camera properties
        shader.setUniform("viewPosition", cam->getPosition())
                .setUniform("view", cam->getViewMatrix())
                .setUniform("projection", cam->getProjectionMatrix());

        // Render cubes
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glBindVertexArray(cubeVao);

        shader.setUniform("model", cube1.getModelMatrix());
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

        shader.setUniform("model", cube2.getModelMatrix());
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

        // Render plane
        glBindTexture(GL_TEXTURE_2D, planeTexture);
        glBindVertexArray(planeVao);

        shader.setUniform("model", plane.getModelMatrix());
        glDrawElements(GL_TRIANGLES, planeIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVao);
    glDeleteBuffers(1, &cubeVbo);
    glDeleteBuffers(1, &cubeEbo);

    glDeleteVertexArrays(1, &planeVao);
    glDeleteBuffers(1, &planeVbo);
    glDeleteBuffers(1, &planeEbo);

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

unsigned int loadTexture(const std::string& imageFilePath) {
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
