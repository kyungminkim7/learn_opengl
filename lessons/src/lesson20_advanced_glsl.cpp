/// Follows "Advanced GLSL" lesson.
/// Creates 4 objects using 4 different shaders and uniform blocks.

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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <stb_image.h>

#include <gl_util/camera.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>
#include <gl_util/uniform_buffer.h>

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

static std::unique_ptr<gl::Camera> cam;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
unsigned int loadTexture(const std::string &imageFilePath);
unsigned int loadCubemapTexture(const std::vector<std::string> &faces);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 20 - Advanced GLSL");
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

        gl::ShaderProgram blueShader(srcDir + "lesson20_default.vert",
                                     srcDir + "lesson20_blue.frag");
        gl::ShaderProgram greenShader(srcDir + "lesson20_default.vert",
                                      srcDir + "lesson20_green.frag");
        gl::ShaderProgram redShader(srcDir + "lesson20_default.vert",
                                    srcDir + "lesson20_red.frag");
        gl::ShaderProgram yellowShader(srcDir + "lesson20_default.vert",
                                       srcDir + "lesson20_yellow.frag");

        // Setup uniform buffer objects
        constexpr auto mat4Size_bytes = sizeof(glm::mat4);
        std::string matricesUniformBlockName = "Matrices";
        gl::UniformBuffer matricesUbo(2 * mat4Size_bytes);
        blueShader.setUniformBlockBinding(matricesUniformBlockName, matricesUbo.getBindingPoint());
        greenShader.setUniformBlockBinding(matricesUniformBlockName, matricesUbo.getBindingPoint());
        redShader.setUniformBlockBinding(matricesUniformBlockName, matricesUbo.getBindingPoint());
        yellowShader.setUniformBlockBinding(matricesUniformBlockName, matricesUbo.getBindingPoint());

        // Setup camera
        cam = std::make_unique<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                           0.1f, 100.0f);
        cam->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
        cam->setLookAtPoint(glm::vec3(0.0f));
        cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

        // Construct game objects
        gl::GameObject blueCube, greenCube, redCube, yellowCube;
        gl::Buffers cubeBuffers(cubeVertices, cubeIndices);

        blueCube.setPosition({-0.75f, 0.75f, 0.0f});
        greenCube.setPosition({0.75f, 0.75f, 0.0f});
        redCube.setPosition({-0.75f, -0.75f, 0.0f});
        yellowCube.setPosition({0.75f, -0.75f, 0.0f});

        auto lastUpdateTime = std::chrono::system_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto currentUpdateTime = std::chrono::system_clock::now();
            auto updateDuration = currentUpdateTime - lastUpdateTime;
            lastUpdateTime = currentUpdateTime;

            cam->onUpdate(updateDuration);
            auto camViewMatrix = cam->getViewMatrix();
            auto camProjectionMatrix = cam->getProjectionMatrix();

            // Set global uniform block data
            matricesUbo.bufferSubData(0, mat4Size_bytes, glm::value_ptr(camViewMatrix));
            matricesUbo.bufferSubData(mat4Size_bytes, mat4Size_bytes, glm::value_ptr(camProjectionMatrix));

            // Render
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render cubes
            glBindVertexArray(cubeBuffers.vao);

            blueShader.use();
            blueCube.render(&blueShader);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            greenShader.use();
            greenCube.render(&greenShader);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            redShader.use();
            redCube.render(&redShader);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            yellowShader.use();
            yellowCube.render(&yellowShader);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()),
                           GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
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

unsigned int loadCubemapTexture(const std::vector<std::string> &faces) {
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    int width, height, numChannels;
    for (unsigned int i = 0; i < faces.size(); ++i) {
        auto data = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load skybox texture at: " << faces[i] << "\n";
            stbi_image_free(data);
            return textureId;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureId;
}
