/// Follows "Shaders" lesson.
/// Creates a multicolored triangle using shaders.

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>
#include <cmath>
#include <array>

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>& a) {
    return N * sizeof(T);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    //Initialize system and window
    gl::initializeGLFW(3, 3);

    const unsigned int windowWidth = 800, windowHeight = 600;
    auto window = gl::createWindow(windowWidth, windowHeight, "Lesson 2 - Shaders");
    if (window == nullptr) return -1;

    if (!gl::initializeGLAD()) return -1;

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCallback);

    gl::ShaderProgram shaderProgram("../../learn_opengl/lessons/src/lesson2_more_attribs.vert",
                                    "../../learn_opengl/lessons/src/lesson2_more_attribs.frag");

    std::array<float, 3 * 6> vertices = {
        //pos              //colors
        0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,//Top
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, //Bottom right
       -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f //Bottom left
    };

    std::array<unsigned int, 3> indices = {
        0, 1, 2, //1st triangle
    };

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    //Copy vertex & index data into GPU buffer's memory
    glBufferData(GL_ARRAY_BUFFER, arrayDataSize(vertices), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, arrayDataSize(indices), indices.data(), GL_STATIC_DRAW);

    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    //Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); //can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind after VAO since VAO records all ebo binds/unbinds

    shaderProgram.use();
//    shaderProgram.setUniform3f("posOffset", 0.25f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
