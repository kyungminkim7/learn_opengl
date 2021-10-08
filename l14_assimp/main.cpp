#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <lgl/Camera.h>
#include <lgl/GameObject.h>
#include <lgl/ShaderProgram.h>
#include <lgl/Texture2D.h>

using Clock = std::chrono::steady_clock;

std::unique_ptr<lgl::Camera> cam;

static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void cursorPosCallback(GLFWwindow *window, double x, double y);
static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

int main(int argc, char *argv[]) {
    // Initialize OpenGL context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int windowWidth = 800;
    const int windowHeight = 600;
    auto window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Set window callbacks
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    glfwSetKeyCallback(window, keyCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    glfwSetScrollCallback(window, scrollCallback);

    // Initialize OpenGL settings
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);

    {
        // Create shaders
        lgl::ShaderProgram shaderProgram("default.vert", "default.frag");
        lgl::ShaderProgram lightShaderProgram("default.vert", "light.frag");

        lgl::GameObject gameObject("../models/nanosuit/nanosuit.obj");
        gameObject.setScale(glm::vec3(0.2f));

        // Setup light
        std::vector<float> vertices {
            // positions
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
        const auto vertexSize = sizeof(decltype(vertices)::value_type);
        const auto stride = 3 * vertexSize;

        unsigned int lightVao;
        glGenVertexArrays(1, &lightVao);
        glBindVertexArray(lightVao);

        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * vertexSize, vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, static_cast<void *>(0));
        glEnableVertexAttribArray(0);

        std::vector<glm::vec3> pointLightPositions {
            { 0.7f,  0.2f,  2.0f},
            { 2.3f, -3.3f, -4.0f},
            {-4.0f,  2.0f, -12.0f},
            { 0.0f,  0.0f, -3.0f}
        };

        std::vector<lgl::Frame> pointLightFrames(pointLightPositions.size());
        auto posIter = pointLightPositions.cbegin();
        for (auto &f : pointLightFrames) {
            f.setPosition(*posIter++);
            f.setScale(glm::vec3(0.2f));
        }

        // Setup camera
        cam = std::make_unique<lgl::Camera>(glm::radians(45.0f), static_cast<float>(windowWidth) / windowHeight, 0.1f, 100.0f);
        cam->setPosition({0.0f, 0.0f, 10.0f});
        cam->rotate(glm::radians(90.0f), {0.0f, 1.0f, 0.0f});
        cam->rotateInLocalFrame(glm::radians(-90.0f), {1.0f, 0.0f, 0.0f});

        shaderProgram.use();
        shaderProgram.setUniform("material.shine", 32.0f);

        shaderProgram.setUniform("directionalLight.direction", {-0.2f, -1.0f, -0.3f});
        shaderProgram.setUniform("directionalLight.lighting.ambient", glm::vec3(0.2f));
        shaderProgram.setUniform("directionalLight.lighting.diffuse", glm::vec3(0.5f));
        shaderProgram.setUniform("directionalLight.lighting.specular", glm::vec3(1.0f));

        for (auto i = 0u; i < pointLightFrames.size(); ++i) {
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].position", pointLightFrames[i].getPosition());
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].linear", 0.09f);
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].lighting.ambient", glm::vec3(0.05f));
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].lighting.diffuse", glm::vec3(0.4f));
            shaderProgram.setUniform("pointLights[" + std::to_string(i) + "].lighting.specular", glm::vec3(0.5f));
        }

        auto lastUpdateTime = Clock::now();
        while (!glfwWindowShouldClose(window)) {
            auto currentUpdateTime = Clock::now();
            auto updateDuration = currentUpdateTime - lastUpdateTime;
            lastUpdateTime = currentUpdateTime;

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cam->onUpdate(updateDuration);
            const auto view_projection_matrix = cam->getProjectionMatrix() * cam->getViewMatrix();

            // Draw cubes
            shaderProgram.use();
            auto camPosition = cam->getPosition();
            shaderProgram.setUniform("camPosition", camPosition);
            shaderProgram.setUniform("view_projection", view_projection_matrix);

            gameObject.render(&shaderProgram);

            // Draw lights
            for (const auto &f : pointLightFrames) {
                lightShaderProgram.use();
                lightShaderProgram.setUniform("view_projection", view_projection_matrix);
                lightShaderProgram.setUniform("model", f.getModelMatrix());
                glBindVertexArray(lightVao);
                glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        cam.reset();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    static const auto FORWARD_KEY = GLFW_KEY_W;
    static const auto BACK_KEY = GLFW_KEY_S;
    static const auto LEFT_KEY = GLFW_KEY_A;
    static const auto RIGHT_KEY = GLFW_KEY_D;
    static const auto speed = 10.0f;

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, true); break;
        case FORWARD_KEY: cam->setLocalSpeedX(speed); break;
        case BACK_KEY: cam->setLocalSpeedX(-speed); break;
        case LEFT_KEY: cam->setLocalSpeedY(speed); break;
        case RIGHT_KEY: cam->setLocalSpeedY(-speed); break;
        default: break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
        case FORWARD_KEY:
            switch (glfwGetKey(window, BACK_KEY)) {
            case GLFW_PRESS: cam->setLocalSpeedX(-speed); break;
            case GLFW_RELEASE: cam->setLocalSpeedX(0.0f); break;
            default: break;
            }
            break;

        case BACK_KEY:
            switch (glfwGetKey(window, FORWARD_KEY)) {
            case GLFW_PRESS: cam->setLocalSpeedX(speed); break;
            case GLFW_RELEASE: cam->setLocalSpeedX(0.0f); break;
            default: break;
            }
            break;

        case LEFT_KEY:
            switch (glfwGetKey(window, RIGHT_KEY)) {
            case GLFW_PRESS: cam->setLocalSpeedY(-speed); break;
            case GLFW_RELEASE: cam->setLocalSpeedY(0.0f); break;
            default: break;
            }
            break;

        case RIGHT_KEY:
            switch (glfwGetKey(window, LEFT_KEY)) {
            case GLFW_PRESS: cam->setLocalSpeedY(speed); break;
            case GLFW_RELEASE: cam->setLocalSpeedY(0.0f); break;
            default: break;
            }
            break;

        default: break;
        }
    }
}

void cursorPosCallback(GLFWwindow *window, double x, double y) {
    static const auto sensitivity = 0.002f;
    static auto lastX = x;
    static auto lastY = y;

    const auto offsetX = x - lastX;
    const auto offsetY = y - lastY;
    lastX = x;
    lastY = y;

    cam->rotateInLocalFrame(offsetY * sensitivity, {0.0f, 1.0f, 0.0f});
    cam->rotate(offsetX * sensitivity, {0.0f, -1.0f, 0.0f});
}

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    static const auto minFOV_deg = 1.0f;
    static const auto maxFOV_deg = 45.0f;
    auto fov_deg = glm::degrees(cam->getFOV()) - static_cast<float>(yOffset);
    fov_deg = std::max(minFOV_deg, fov_deg);
    fov_deg = std::min(maxFOV_deg, fov_deg);
    cam->setFOV(glm::radians(fov_deg));
}
