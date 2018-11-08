/// Follows "Framebuffers" lesson.
/// Renders scene onto a separate framebuffer for post-processing.
/// This screen shader implements an edge-detection kernel.

#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <stb_image.h>

#include <gl_util/camera.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>

namespace gl {
struct Buffers {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    Buffers(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
        glGenVertexArrays(1, &this->vao);
        glBindVertexArray(this->vao);

        glGenBuffers(1, &this->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

        glGenBuffers(1, &this->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);

        // Copy vertex & index data into GPU buffer's memory
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof (float)),
                     vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof (unsigned int)),
                     indices.data(), GL_STATIC_DRAW);

        // Define vertex attributes
        const auto stride = 5 * sizeof (float);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); // can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind after VAO since VAO records all ebo binds/unbinds
    }

    ~Buffers() {
        glDeleteBuffers(1, &this->ebo);
        glDeleteBuffers(1, &this->vbo);
        glDeleteVertexArrays(1, &this->vao);
    }
};
}

const static std::vector<float> cubeVertices {
    // position             // tex coord
    // Front face
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // Top left
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // Bottom left
    0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // Top right
    0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // Bottom right

    // Left face
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // Top left
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // Bottom left
    -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // Top right
    -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // Bottom right

    // Right face
    0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // Top left
    0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // Bottom left
    0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // Top right
    0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // Bottom right

    // Back face
    0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // Top left
    0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // Bottom left
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // Top right
    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // Bottom right

    // Top face
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // Top left
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, // Bottom left
    0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // Top right
    0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // Bottom right

    // Bottom face
    -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, // Top left
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // Bottom left
    0.5f, -0.5f,  0.5f,    1.0f, 1.0f, // Top right
    0.5f, -0.5f, -0.5f,    1.0f, 0.0f // Bottom right
};

const static std::vector<unsigned int> cubeIndices {
    0, 1, 2,
    3, 2, 1,

    4, 5, 6,
    7, 6, 5,

    8, 9, 10,
    11, 10, 9,

    12, 13, 14,
    15, 14, 13,

    16, 17, 18,
    19, 18, 17,

    20, 21, 22,
    23, 22, 21
};

const static std::vector<float> planeVertices {
    // positions          // texture Coords
    5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f
};

const static std::vector<unsigned int> planeIndices {
    0, 1, 2,
    3, 2, 1
};

const static std::vector<float> screenVertices {
    // position      // tex coord
    -1.0f,  1.0f,    0.0f, 1.0f, // Top left
    -1.0f, -1.0f,    0.0f, 0.0f, // Bottom left
     1.0f,  1.0f,    1.0f, 1.0f, // Top right
     1.0f, -1.0f,    1.0f, 0.0f, // Bottom right
};

const static std::vector<unsigned int> screenIndices {
    0, 1, 2,
    3, 2, 1
};

const static std::vector<glm::vec3> glassPositions {
    {-1.5f, 0.0f, -0.48f},
    {1.5f, 0.0f, 0.51f},
    {0.0f, 0.0f, 0.7f},
    {-0.3f, 0.0f, -2.3f},
    {0.5f, 0.0f, -0.6f}
};

static std::unique_ptr<gl::Camera> cam;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
unsigned int loadTexture(const std::string& imageFilePath);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 18 - Framebuffers");
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
    {
        std::string baseDir("../../learn_opengl/lessons/");
        auto srcDir = baseDir + "src/";

        gl::ShaderProgram sceneShader(srcDir + "lesson18_scene.vert",
                                      srcDir + "lesson18_scene.frag");
        gl::ShaderProgram screenShader(srcDir + "lesson18_screen.vert",
                                       srcDir + "lesson18_screen.frag");

        // Set up FBO
        unsigned int fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // FBO color texture attachment
        unsigned int fboTexture;
        glGenTextures(1, &fboTexture);
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameBufferWidth, frameBufferHeight, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // FBO depth and stencil RBO attachment
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameBufferWidth, frameBufferHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR - Framebuffer is not complete!\n";
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Set up camera
        cam = std::make_unique<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                           0.1f, 100.0f);
        cam->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
        cam->setLookAtPoint(glm::vec3(0.0f));
        cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

        // Construct cubes
        gl::Buffers cubeBuffers(cubeVertices, cubeIndices);
        auto cubeTexture = loadTexture(baseDir + "images/container.jpg");

        gl::GameObject cube1, cube2;
        cube1.setPosition({-1.0f, 0.0f, -1.0f});
        cube2.setPosition({2.0f, 0.0f, 0.0f});

        // Construct plane
        gl::Buffers planeBuffers(planeVertices, planeIndices);
        auto planeTexture = loadTexture(baseDir + "images/metal.png");

        gl::GameObject plane;

        // Construct screen quad
        unsigned int screenVao, screenVbo, screenEbo;
        glGenVertexArrays(1, &screenVao);
        glBindVertexArray(screenVao);

        glGenBuffers(1, &screenVbo);
        glBindBuffer(GL_ARRAY_BUFFER, screenVbo);

        glGenBuffers(1, &screenEbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenEbo);

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(screenVertices.size() * sizeof(float)),
                     screenVertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(screenIndices.size() * sizeof(float)),
                     screenIndices.data(), GL_STATIC_DRAW);

        const auto stride = 4 * sizeof(float);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), reinterpret_cast<GLvoid*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        sceneShader.use();
        glActiveTexture(GL_TEXTURE0);
        sceneShader.setUniform("texture0", 0);

        screenShader.use();
        glActiveTexture(GL_TEXTURE0);
        screenShader.setUniform("screenTexture", 0);

        auto lastUpdateTime = std::chrono::system_clock::now();

        while (!glfwWindowShouldClose(window)) {
            // Update game components
            auto currentUpdateTime = std::chrono::system_clock::now();
            auto updateDuration = currentUpdateTime - lastUpdateTime;
            lastUpdateTime = currentUpdateTime;

            cam->onUpdate(updateDuration);

            // Render scene onto FBO
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            sceneShader.use();

            // Set camera properties
            sceneShader.setUniform("viewPosition", cam->getPosition())
                    .setUniform("view", cam->getViewMatrix())
                    .setUniform("projection", cam->getProjectionMatrix());

            // Render plane
            glBindTexture(GL_TEXTURE_2D, planeTexture);
            glBindVertexArray(planeBuffers.vao);

            sceneShader.setUniform("model", plane.getModelMatrix());
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(planeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            // Render cubes
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glBindVertexArray(cubeBuffers.vao);

            sceneShader.setUniform("model", cube1.getModelMatrix());
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            sceneShader.setUniform("model", cube2.getModelMatrix());
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            // Render FBO scene onto screen quad for post-processing
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            screenShader.use();
            glBindTexture(GL_TEXTURE_2D, fboTexture);
            glBindVertexArray(screenVao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(screenIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteBuffers(1, &screenVbo);
        glDeleteBuffers(1, &screenEbo);
        glDeleteVertexArrays(1, &screenVao);

        glDeleteRenderbuffers(1, &rbo);
        glDeleteTextures(1, &fboTexture);
        glDeleteFramebuffers(1, &fbo);
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

unsigned int loadTexture(const std::string& imageFilePath) {
    unsigned int textureId;
    glGenTextures(1, &textureId);

    stbi_set_flip_vertically_on_load(true);

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
                 0, static_cast<GLint>(format), width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureId;
}
