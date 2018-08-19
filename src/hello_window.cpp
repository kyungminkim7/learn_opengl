#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>

constexpr unsigned int LOG_LENGTH = 512;

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 600;

const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n\n"
        "void main(){\n"
        "\tgl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
        "}";

const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 fragColor;\n\n"
        "void main(){\n"
        "fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}";

int createShaderProgram();
unsigned int compileShader(int shaderType, const char *shaderSource, unsigned int numSourceCodeStr);
void frameBufferSizeCb(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    //Initialize/configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Create window
    auto window = glfwCreateWindow(windowWidth, windowHeight, "Learn OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Initialize GLAD to manage OS-specific OpenGL fxn ptrs
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    auto shaderProgram = createShaderProgram();

    //Setup vertex data to draw
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    //Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Setup window
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCb);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}

int createShaderProgram() {
    auto vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, 1);
    auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, 1);

    //Link compiled shaders
    int linkSuccess;
    char linkLog[LOG_LENGTH];
    auto shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        glGetProgramInfoLog(shaderProgram, LOG_LENGTH, nullptr, linkLog);
        std::cout << "ERROR::SHADER::LINKING_FAILED\n" << linkLog << "\n";
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

void frameBufferSizeCb(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
