///Draw 2 triangles next to each other using glDrawArrays
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <vector>

constexpr unsigned int LOG_LENGTH = 512;

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 600;

const char *identityVertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n\n"
        "void main(){\n"
        "\tgl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
        "}";

const char *orangeFragmentShaderSource =
        "#version 330 core\n"
        "out vec4 fragColor;\n\n"
        "void main(){\n"
        "fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}";

const char *redFragmentShaderSource =
        "#version 330 core\n"
        "out vec4 fragColor;\n\n"
        "void main(){\n"
        "fragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
        "}";

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>& a) {
    return N * sizeof(T);
}

void initializeGLFW();
GLFWwindow* createWindow();
int createShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource);
unsigned int compileShader(int shaderType, const char *shaderSource, unsigned int numSourceCodeStr);
void frameBufferSizeCb(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    initializeGLFW();
    auto window = createWindow();
    if (window == nullptr) return -1;
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    std::vector<int> shaderPrograms ={
        createShaderProgram(identityVertexShaderSource, orangeFragmentShaderSource),
        createShaderProgram(identityVertexShaderSource, redFragmentShaderSource)
    };

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCb);

    std::vector<std::array<float, 9>> vertices = {
        // first triangle
        {
            -0.9f, -0.5f, 0.0f,  // left
            -0.0f, -0.5f, 0.0f,  // right
            -0.45f, 0.5f, 0.0f,  // top
        },

        // second triangle
        {
            0.0f, -0.5f, 0.0f,  // left
            0.9f, -0.5f, 0.0f,  // right
            0.45f, 0.5f, 0.0f   // top
        }
    };

    std::array<unsigned int, 2> vaos, vbos;
    glGenVertexArrays(vaos.size(), vaos.data());
    glGenBuffers(vbos.size(), vbos.data());

    for (size_t i = 0; i < vertices.size(); i++) {
        glBindVertexArray(vaos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);

        glBufferData(GL_ARRAY_BUFFER, arrayDataSize(vertices[i]), vertices[i].data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (size_t i = 0; i < vaos.size(); i++) {
            glUseProgram(shaderPrograms[i]);
            glBindVertexArray(vaos[i]);
            glDrawArrays(GL_TRIANGLES, 0, vertices[i].size());
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(vaos.size(), vaos.data());
    glDeleteBuffers(vbos.size(), vbos.data());

    glfwTerminate();
    return 0;
}

void initializeGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWindow() {
    auto window = glfwCreateWindow(windowWidth, windowHeight, "Learn OpenGL", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
    }

    return window;
}

int createShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource) {
    auto vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, 1);
    auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, 1);

    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    int linkSuccess;
    char linkLog[LOG_LENGTH];
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        glGetProgramInfoLog(shaderProgram, LOG_LENGTH, nullptr, linkLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << linkLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int compileShader(int shaderType, const char *shaderSource, unsigned int numSourceCodeStr) {
    int compileSuccess;
    char compileLog[LOG_LENGTH];

    auto shader = glCreateShader(shaderType);
    glShaderSource(shader, numSourceCodeStr, &shaderSource, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (!compileSuccess) {
        glGetShaderInfoLog(shader, LOG_LENGTH, nullptr, compileLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << compileLog << "\n";
    }

    return shader;
}

void frameBufferSizeCb(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
