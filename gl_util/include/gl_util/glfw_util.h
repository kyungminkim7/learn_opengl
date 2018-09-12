#pragma once

///Helper functions for GLFW and GLAD.

#include <string>

class GLFWwindow;

namespace gl {

void initializeGLFW(unsigned int contextMajorVersion, unsigned int contextMinorVersion);
bool initializeGLAD();
GLFWwindow* createWindow(unsigned int width, unsigned int height, std::string title);

}
