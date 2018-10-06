/// Follows "Coordinate Systems" lesson.
/// Draws multiple boxes using model, view and projection matrices.

#include <array>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>

float texMix = 0.2;

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>& a) {
    return N * sizeof(T);
}

void frameBufferSizeCb(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, gl::ShaderProgram* shaderProgram);

int main() {
    // Initialize OpenGL and window
    gl::initializeGLFW(3, 3);

    const unsigned int windowWidth = 800, windowHeight = 600;
    auto window = gl::createWindow(windowWidth, windowHeight, "Lesson 4 - Transformations");
    if (window == nullptr) return -1;

    if (!gl::initializeGLAD()) return -1;

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCb);

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    gl::ShaderProgram shaderProgram("../../learn_opengl/lessons/src/lesson5_coordinate_systems.vert",
                                    "../../learn_opengl/lessons/src/lesson5_coordinate_systems.frag");

    constexpr std::size_t numAttributes = 5;
    constexpr auto numFaces = 6;
    constexpr auto numVerticesPerFace = 4;

    // Cube vertex data
    std::array<float, numFaces * numVerticesPerFace * numAttributes> vertices = {
        //pos                  //tex coord
        //Front face
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, //Top left
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, //Top right
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f, //Bottom right
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  //Bottom left

        //Left face
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, //Top left
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, //Top right
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, //Bottom right
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, //Bottom left

        //Right face
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, //Top left
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, //Top right
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, //Bottom right
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f, //Bottom left

        //Back face
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, //Top left
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, //Top right
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, //Bottom right
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  //Bottom left

        //Top face
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, //Top left
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, //Top right
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f, //Bottom right
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, //Bottom left

        //Bottom face
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, //Top left
         0.5f, -0.5f,  0.5f,    1.0f, 1.0f, //Top right
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, //Bottom right
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f  //Bottom left
    };

    constexpr auto trianglesPerFace = 2;
    constexpr auto verticesPerTriangle = 3;

    // Cube
    std::array<unsigned int, numFaces * trianglesPerFace * verticesPerTriangle> indices = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    std::vector<glm::vec3> cubePositions {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f, 3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f, 2.0f, -2.5f},
        {1.5f, 0.2f, -1.5f},
        {-1.3f, 1.0f, -1.5f}
    };

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Copy vertex & index data into GPU buffer's memory
    glBufferData(GL_ARRAY_BUFFER, arrayDataSize(vertices), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, arrayDataSize(indices), indices.data(), GL_STATIC_DRAW);

    auto stride = numAttributes * sizeof(float);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); //can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind after VAO since VAO records all ebo binds/unbinds

    // Load & create texture0
    unsigned int texture0;
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    {
        int imgWidth, imgHeight, numImgChannels;
        const auto imgFilepath = "../../learn_opengl/lessons/images/container.jpg";
        auto img = stbi_load(imgFilepath, &imgWidth, &imgHeight, &numImgChannels, 0);
        if (!img) {
            std::cerr << "Failed to load img from file: " << imgFilepath << "\n";
            return -1;
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0, GL_RGB, imgWidth, imgHeight, 0, //Texture attributes
                     GL_RGB, GL_UNSIGNED_BYTE, img);    //Src img attributes
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(img);
    }

    // Load & create texture1;
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    {
        int imgWidth, imgHeight, numImgChannels;
        const auto imgFilepath = "../../learn_opengl/lessons/images/awesomeface.png";
        auto img = stbi_load(imgFilepath, &imgWidth, &imgHeight, &numImgChannels, 0);
        if (!img) {
            std::cerr << "Failed to load img from file: " << imgFilepath << "\n";
            return -1;
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0, GL_RGB, imgWidth, imgHeight, 0, //Texture attributes
                     GL_RGBA, GL_UNSIGNED_BYTE, img);    //Src img attributes
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(img);
    }

    // Camera perspective properties
    const auto fov = glm::radians(45.0f);
    const auto aspectRatio = static_cast<float>(windowWidth) / windowHeight;
    const auto nearPlane = 0.1f;
    const auto farPlane = 100.0f;

    shaderProgram.use();
    shaderProgram.setUniform("texture0", 0);
    shaderProgram.setUniform("texture1", 1);
    shaderProgram.setUniform("texMix", texMix);

    // Set camera view and projection matrices
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    shaderProgram.setUniform("view", view);

    auto projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    shaderProgram.setUniform("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        processInput(window, &shaderProgram);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glBindVertexArray(vao);

        // Draw multiple rotating cubes
        for (int i = 0; i < cubePositions.size(); ++i) {
            auto angle = glm::radians(i * 20.0f) +
                    (i % 3 == 0 ? static_cast<float>(glfwGetTime()) * glm::radians(50.0f) : 0);

            glm::mat4 model(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

            shaderProgram.setUniform("model", model);

            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}

void frameBufferSizeCb(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, gl::ShaderProgram* shaderProgram) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        texMix += (texMix < 1 ? 0.1 : 0);
        shaderProgram->use();
        shaderProgram->setUniform("texMix", texMix);
    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        texMix -= (texMix > 0 ? 0.1 : 0);
        shaderProgram->use();
        shaderProgram->setUniform("texMix", texMix);
    }
}
