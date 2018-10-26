/// Follows "Hello Window" and "Hello Triangle" lessons.
/// Creates a basic window and rectangle from 2 triangles.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <type_traits>

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

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>& a) {
    return N * sizeof(T);
}

int createShaderProgram();
unsigned int compileShader(int shaderType, const char *shaderSource, unsigned int numSourceCodeStr);
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
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

    //Rectangle vertices
    std::array<float, 12> vertices = {
        0.5f,  0.5f, 0.0f, //Top right
        0.5f, -0.5f, 0.0f, //Bottom right
       -0.5f, -0.5f, 0.0f, //Bottom left
       -0.5f,  0.5f, 0.0f, //Top left
    };

    std::array<unsigned int, 6> indices = {
        0, 1, 3, //1st triangle
        1, 2, 3, //2nd triangle
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

    //Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    //Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); //can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind after VAO since VAO records all ebo binds/unbinds

    //Setup window
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCallback);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        static bool toggle = false;
        toggle = !toggle;
        glPolygonMode(GL_FRONT_AND_BACK, toggle ? GL_FILL : GL_LINE);
    }
}
