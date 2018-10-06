/// Follows "Textures" lesson.
/// Loads two images as textures and mixes them over a square.
/// Press the UP/DOWN key to vary the amount of texture mixture;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl_util/glfw_util.h>
#include <gl_util/shader_program.h>
#include <stb_image.h>
#include <iostream>
#include <array>

float texMix = 0.2;

template<typename T, std::size_t N>
unsigned int arrayDataSize(const std::array<T,N>& a) {
    return N * sizeof(T);
}

void frameBufferSizeCb(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, gl::ShaderProgram* shaderProgram);

int main() {
    //Initialize system and window
    gl::initializeGLFW(3, 3);

    const unsigned int windowWidth = 800, windowHeight = 600;
    auto window = gl::createWindow(windowWidth, windowHeight, "Lesson 3 - Textures");
    if (window == nullptr) return -1;

    if (!gl::initializeGLAD()) return -1;

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCb);

    gl::ShaderProgram shaderProgram("../../learn_opengl/lessons/src/lesson3_texture_units.vert",
                                    "../../learn_opengl/lessons/src/lesson3_texture_units.frag");

    constexpr std::size_t numAttributes = 8;

    //Setup vertex data
    std::array<float, 4 * numAttributes> vertices = {
        //pos                //colors            //tex coord
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 2.0f, //Top left
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   2.0f, 2.0f, //Top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, //Bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f  //Bottom left
    };

    //rectangle
    std::array<unsigned int, 2 * 3> indices = {
        0, 1, 2,
        2, 3, 0,
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

    auto stride = numAttributes * sizeof(float);

    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    //Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); //can unbind before VAO since glVertexAttribPointer registered VBO as bound buffer
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind after VAO since VAO records all ebo binds/unbinds

    //Load & create texture1
    unsigned int texture0;
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

    //Load & create texture2;
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

    shaderProgram.use();
    shaderProgram.setUniform("texture0", 0);
    shaderProgram.setUniform("texture1", 1);
    shaderProgram.setUniform("texMix", texMix);

    while (!glfwWindowShouldClose(window)) {
        processInput(window, &shaderProgram);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}

void frameBufferSizeCb(GLFWwindow* window, int width, int height) {
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
