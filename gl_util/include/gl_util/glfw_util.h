#pragma once

///
/// \brief Helper functions for GLFW and GLAD.
///
/// This module provides functions for initializing the windowing system and loading
/// OpenGL methods. To initialize and obtain an OpenGL context, users should call
/// the functions in the following order:
///     1. initializeGLFW
///     2. createWindow
///     3. initializeGLAD
///

#include <string>

class GLFWwindow;

namespace gl {

void initializeGLFW(int contextMajorVersion, int contextMinorVersion);
GLFWwindow* createWindow(int width, int height, const std::string &title);
bool initializeGLAD();

} // namespace gl
