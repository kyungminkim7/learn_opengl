/// Follows "Advanced Lighting" lesson.
/// Switches between Phong and Blinn-Phong shading when 'b' button is pressed.

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
#include <gl_util/instancing_game_objects.h>
#include <gl_util/game_object.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>
#include <gl_util/uniform_buffer.h>

static std::unique_ptr<gl::Camera> cam;

bool blinn = false;

const static std::vector<float> planeVertices {
    // positions            // normals         // texcoords
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
unsigned int loadTexture(const std::string &imageFilePath);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 24 - Advanced Lighting");
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

        gl::ShaderProgram defaultShader(srcDir + "lesson24_default.vert",
                                        srcDir + "lesson24_default.frag");

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

        // Setup plane
        constexpr auto planeElemSize_bytes = sizeof(float);
        constexpr auto stride = 8 * planeElemSize_bytes;
        unsigned int planeVao, planeVbo;
        glGenVertexArrays(1, &planeVao);
        glGenBuffers(1, &planeVbo);

        glBindVertexArray(planeVao);
        glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
        glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * planeElemSize_bytes,
                     planeVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              stride, reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              stride, reinterpret_cast<GLvoid*>(3 * planeElemSize_bytes));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                              stride, reinterpret_cast<GLvoid*>(6 * planeElemSize_bytes));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        auto planeTexture = loadTexture(baseDir + "images/wood.png");
        gl::GameObject plane;

        glm::vec3 lightPosition(0.0f);

        auto lastUpdateTime = std::chrono::system_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto currentUpdateTime = std::chrono::system_clock::now();
            auto updateDuration = currentUpdateTime - lastUpdateTime;
            lastUpdateTime = currentUpdateTime;

            // Update
            cam->onUpdate(updateDuration);

            // Render
            matricesUbo.bufferSubData(0, mat4Size_bytes, glm::value_ptr(cam->getViewMatrix()))
                    .bufferSubData(mat4Size_bytes, mat4Size_bytes, glm::value_ptr(cam->getProjectionMatrix()));

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            defaultShader.use()
                    .setUniform("blinn", blinn ? 1 : 0);

            // Render light
            defaultShader.setUniform("light.position", lightPosition)
                    .setUniform("light.lighting.ambient", glm::vec3(0.05f))
                    .setUniform("light.lighting.diffuse", glm::vec3(1.0f))
                    .setUniform("light.lighting.specular", glm::vec3(0.3f));

            // Render plane
            defaultShader.setUniform("material.diffuseTexture0", 0)
                    .setUniform("material.shininess", 8.0f);
            plane.render(&defaultShader);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, planeTexture);
            glBindVertexArray(planeVao);
            glDrawArrays(GL_TRIANGLES, 0, planeVertices.size());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteTextures(1, &planeTexture);
        glDeleteBuffers(1, &planeVbo);
        glDeleteVertexArrays(1, &planeVao);
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


    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        blinn = !blinn;
    }
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
