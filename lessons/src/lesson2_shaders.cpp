#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>

void frameBufferSizeCb(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    //Initialize system and window
    glfw_util::initializeGLFW(3, 3);

    const unsigned int windowWidth = 800, windowHeight = 600;
    auto window = glfw_util::createWindow(windowWidth, windowHeight, "Lesson 2 - Shaders");
    if (window == nullptr) return -1;

    if (!glfw_util::initializeGLAD()) return -1;

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCb);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void frameBufferSizeCb(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
