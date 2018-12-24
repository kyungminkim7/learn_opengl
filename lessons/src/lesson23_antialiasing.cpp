/// Follows "Antialiasing" lesson.
/// Uses antialiasing to smooth edges.

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

const static std::vector<float> cubeVertices {
    // Positions
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

const static std::vector<float> quadVertices {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 23 - Antialiasing");
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

        gl::ShaderProgram defaultShader(srcDir + "lesson23_default.vert",
                                        srcDir + "lesson23_default.frag");
        gl::ShaderProgram screenShader(srcDir + "lesson23_default.vert",
                                       srcDir + "lesson23_screen.frag");

        constexpr auto mat4Size_bytes = sizeof(glm::mat4);
        gl::UniformBuffer matricesUbo(2 * mat4Size_bytes);
        defaultShader.setUniformBlockBinding("Matrices", matricesUbo.getBindingPoint());
        screenShader.setUniformBlockBinding("Matrices", matricesUbo.getBindingPoint());

        // Setup camera
        cam = std::make_unique<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                           0.1f, 1000.0f);
        cam->setPosition(glm::vec3(0.0f, 1.0f, 3.0f));
        cam->setLookAtPoint(glm::vec3(0.0f));
        cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

        // Setup multisampled FBO
        constexpr auto numSamples = 4;
        unsigned int fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // FBO color attachment
        unsigned int fboTexture;
        glGenTextures(1, &fboTexture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fboTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB,
                                frameBufferWidth, frameBufferHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D_MULTISAMPLE, fboTexture, 0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        // FBO stencil and depth attachment
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH24_STENCIL8,
                                         frameBufferWidth, frameBufferHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Setup screen
        unsigned int screenVao, screenVbo;
        glGenVertexArrays(1, &screenVao);
        glGenBuffers(1, &screenVbo);

        glBindVertexArray(screenVao);
        glBindBuffer(GL_ARRAY_BUFFER, screenVbo);
        glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(decltype(quadVertices)::value_type),
                     quadVertices.data(), GL_STATIC_DRAW);

        constexpr auto stride = 5 * sizeof(decltype(quadVertices)::value_type);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              stride, reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              stride, reinterpret_cast<GLvoid*>(3 * sizeof(decltype(quadVertices)::value_type)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Setup cube
        gl::GameObject cube;
        unsigned int cubeVao, cubeVbo;

        glGenVertexArrays(1, &cubeVao);
        glGenBuffers(1, &cubeVbo);

        glBindVertexArray(cubeVao);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
        glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(decltype(cubeVertices)::value_type),
                     cubeVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              3 * sizeof(decltype(cubeVertices)::value_type),
                              reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


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

            // Render multisampled data onto FBO
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            defaultShader.use();
            cube.render(&defaultShader);
            glBindVertexArray(cubeVao);
            glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

            // Resolve FBO data onto default screen
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, frameBufferWidth, frameBufferHeight,
                              0, 0, frameBufferWidth, frameBufferHeight,
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteBuffers(1, &cubeVbo);
        glDeleteVertexArrays(1, &cubeVao);

        glDeleteBuffers(1, &screenVbo);
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
