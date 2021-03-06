/// Follows "Shadow Mapping" lesson.
/// Generates shadows using a directional light.

#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
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
#include <gl_util/directional_light.h>
#include <gl_util/instancing_game_objects.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>
#include <gl_util/uniform_buffer.h>

const static std::vector<float> planeVertexPositions {
     25.0f, 0.0f,  25.0f,
    -25.0f, 0.0f,  25.0f,
    -25.0f, 0.0f, -25.0f,
     25.0f, 0.0f, -25.0f
};

const static std::vector<float> planeNormals {
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

const static std::vector<float> planeTextureCoords {
    25.0f,  0.0f,
    0.0f,  0.0f,
    0.0f, 25.0f,
    25.0f, 25.0f
};

const static std::vector<unsigned int> planeIndices {
    0u, 1u, 2u,
    0u, 2u, 3u
};

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

const static unsigned int SHADOW_MAP_WIDTH = 1024;
const static unsigned int SHADOW_MAP_HEIGHT = 1024;

static std::unique_ptr<gl::Camera> cam;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
unsigned int loadTexture(const std::string &imageFilePath);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 25 - Shadow Mapping");
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

        gl::ShaderProgram shadowMapShader(srcDir + "lesson25_shadow_map.vert",
                                          srcDir + "lesson25_shadow_map.frag");
        gl::ShaderProgram defaultShader(srcDir + "lesson25_default.vert",
                                        srcDir + "lesson25_default.frag");

        constexpr auto mat4Size_bytes = sizeof(glm::mat4);
        gl::UniformBuffer matricesUbo(2 * mat4Size_bytes);
        defaultShader.setUniformBlockBinding("Matrices", matricesUbo.getBindingPoint());

        // Setup camera
        cam = std::make_unique<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                           0.1f, 1000.0f);
        cam->setPosition(glm::vec3(0.0f, 1.0f, 3.0f));
        cam->setLookAtPoint(glm::vec3(0.0f));
        cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

        // Create depth map
        unsigned int shadowMapFBO;
        glGenFramebuffers(1, &shadowMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

        unsigned int shadowMapTexture;
        glGenTextures(1, &shadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const std::vector<float> borderColor(4, 1.0f);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.data());

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Setup game objects
        gl::GameObject plane(planeVertexPositions,
                             planeNormals,
                             planeTextureCoords,
                             planeIndices,
                             baseDir + "images/wood.png");
        plane.setPosition({0.0f, -0.5f, 0.0f});

        std::vector<gl::GameObject> cubes(3, gl::GameObject(cubeVertexPositions,
                                                            cubeNormals,
                                                            cubeTextureCoords,
                                                            cubeIndices,
                                                            baseDir + "images/wood.png"));
        cubes[0].setScale(glm::vec3(0.5f));
        cubes[0].setPosition({0.0f, 1.5f, 0.0f});

        cubes[1].setScale(glm::vec3(0.5f));
        cubes[1].setPosition({2.0f, 0.0f, 1.0f});

        cubes[2].setScale(glm::vec3(0.25f));
        cubes[2].setPosition({-1.0f, 0.0f, 2.0f});
        cubes[2].rotate(glm::radians(60.0f), {1.0f, 0.0f, 1.0f});

        gl::DirectionalLight light(-15.0f, 15.0f, -15.0f, 15.0f, 1.0f, 10.0f);
        light.setPosition({-2.0f, 4.0f, -1.0f})
                .setNormalDirection({0.0f, 1.0f, 0.0f})
                .setLookAtPoint(glm::vec3(0.0f));

        auto lastUpdateTime = std::chrono::system_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto currentUpdateTime = std::chrono::system_clock::now();
            auto updateDuration = currentUpdateTime - lastUpdateTime;
            lastUpdateTime = currentUpdateTime;

            // Update
            cam->onUpdate(updateDuration);
            light.setPosition(glm::vec3{-2.0f, 4.0f, -1.0f} +
                              2.0f * static_cast<float>(std::sin(glfwGetTime())) * glm::vec3{1.0f, 0.0f, 1.0f});
            light.setLookAtPoint(glm::vec3(0.0f));

            // Render
            matricesUbo.bufferSubData(0, mat4Size_bytes, glm::value_ptr(cam->getViewMatrix()))
                    .bufferSubData(mat4Size_bytes, mat4Size_bytes, glm::value_ptr(cam->getProjectionMatrix()));

            // 1st render to depth map to generate shadows
            glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            auto lightSpaceMatrix = light.getProjectionMatrix() * light.getViewMatrix();

            shadowMapShader.use();
            shadowMapShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);

            plane.render(&shadowMapShader);

            for (auto& c : cubes) {
                c.render(&shadowMapShader);
            }

            // Render scene with shadows
            glViewport(0, 0, frameBufferWidth, frameBufferHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            defaultShader.use();
            defaultShader.setUniform("viewPosition", cam->getPosition())
                    .setUniform("light.position", light.getPosition())
                    .setUniform("light.lighting.ambient", glm::vec3(0.3f))
                    .setUniform("light.lighting.diffuse", glm::vec3(1.0f))
                    .setUniform("light.lighting.specular", glm::vec3(0.0f))
                    .setUniform("lightSpaceMatrix", lightSpaceMatrix);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
            defaultShader.setUniform("shadowMap", 2);

            // Render plane
            glActiveTexture(GL_TEXTURE0);
            defaultShader.setUniform("material.diffuseTexture0", 0)
                    .setUniform("material.shininess", 64.0f);
            plane.render(&defaultShader);

            for (auto& c : cubes) {
                c.render(&defaultShader);
            }

            glBindTexture(GL_TEXTURE_2D, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteTextures(1, &shadowMapTexture);
        glDeleteFramebuffers(1, &shadowMapFBO);
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
