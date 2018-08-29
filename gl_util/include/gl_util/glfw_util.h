///Helper functions for GLFW and GLAD.
#pragma once
#include <string>

class GLFWwindow;

namespace glfw_util {

void initializeGLFW(unsigned int contextMajorVersion, unsigned int contextMinorVersion);
bool initializeGLAD();
GLFWwindow* createWindow(unsigned int width, unsigned int height, std::string title);

}
