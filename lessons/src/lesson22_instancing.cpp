/// Follows "Instancing" lesson.
/// Uses instanced rendering to draw lots of asteroids sharing the same
/// vertex data.

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

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    auto window = gl::createWindow(800, 600, "Lesson 22 - Instancing");
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

        gl::ShaderProgram defaultShader(srcDir + "lesson22_default.vert",
                                        srcDir + "lesson22_default.frag");
        gl::ShaderProgram instancingShader(srcDir + "lesson22_instancing.vert",
                                           srcDir + "lesson22_default.frag");

        constexpr auto mat4Size_bytes = sizeof(glm::mat4);
        gl::UniformBuffer matricesUbo(2 * mat4Size_bytes);
        defaultShader.setUniformBlockBinding("Matrices", matricesUbo.getBindingPoint());
        instancingShader.setUniformBlockBinding("Matrices", matricesUbo.getBindingPoint());

        // Setup camera
        cam = std::make_unique<gl::Camera>(45.0f, static_cast<float>(frameBufferWidth) / frameBufferHeight,
                                           0.1f, 1000.0f);
        cam->setPosition(glm::vec3(0.0f, 30.0f, 250.0f));
        cam->setLookAtPoint(glm::vec3(0.0f));
        cam->setNormalDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setHorizontalRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));
        cam->setLinearSpeed(25.0f);

        // Setup planet
        auto modelsDir = baseDir + "models/";
        gl::GameObject planet(modelsDir + "planet/planet.obj");
        planet.setPosition({0.0f, -3.0f, 0.0f});
        planet.setScale(glm::vec3(4.0f));

        // Setup asteroids
        constexpr auto offset = 25.0f;
        constexpr auto radius = 150.0f;

        std::random_device rd;
        std::uniform_real_distribution<float> randDisplacement(-offset, offset);
        std::uniform_real_distribution<float> randAngle_rad(0.0f, 2 * glm::pi<float>());
        std::uniform_real_distribution<float> randScale(0.05f, 0.25f);

        gl::InstancingGameObjects asteroids(modelsDir + "rock/rock.obj", 100000);
        for (auto i = 0ul; i < asteroids.size(); ++i) {
            auto angle_rad = static_cast<float>(i) / asteroids.size() * 2 * glm::pi<float>();

            asteroids[i].setPosition({std::sin(angle_rad) * radius + randDisplacement(rd),
                                      randDisplacement(rd) * 0.4f,
                                      std::cos(angle_rad) * radius + randDisplacement(rd)});
            asteroids[i].rotate(randAngle_rad(rd), {0.4f, 0.6f, 0.8f});
            asteroids[i].setScale(glm::vec3(randScale(rd)));
        }

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

            defaultShader.use();
            planet.render(&defaultShader);

            instancingShader.use();
            asteroids.render(&instancingShader);

            glfwSwapBuffers(window);
            glfwPollEvents();

            auto fps = 1.0f / std::chrono::duration<float>(updateDuration).count();
            std::cout << "FPS: " << fps << " hz\n";
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
