#include <gl_util/glfw_util.h>

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace gl {

void initializeGLFW(int contextMajorVersion, int contextMinorVersion) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, contextMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, contextMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::cout << "Initialized GLFW.\n";
}

GLFWwindow* createWindow(int width, int height, const std::string &title) {
    auto window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    return window;
}

bool initializeGLAD() {
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwSwapInterval(1);
        std::cout << "Initialized GLAD.\n";
        return true;
    } else {
        std::cerr << "Failed to initialize GLAD.\n";
        return false;
    }
}

} // namespace gl
