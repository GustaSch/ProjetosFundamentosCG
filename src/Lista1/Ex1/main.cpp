#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
uniform vec4 uColor;
void main() {
    FragColor = uColor;
}
)";

int drawMode = 4; // 1=preenchido, 2=contorno, 3=pontos, 4=todos juntos

void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) drawMode = 1;
        if (key == GLFW_KEY_2) drawMode = 2;
        if (key == GLFW_KEY_3) drawMode = 3;
        if (key == GLFW_KEY_4) drawMode = 4;
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    }
}

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Erro ao compilar shader: " << infoLog << std::endl;
    }
    return shader;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Exercicio 1 - Dois Triangulos", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Falha ao iniciar GLAD" << std::endl;
        return -1;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertices dos dois triangulos, seguindo a figura do enunciado
    float vertices[] = {
        // Triangulo 1: v0, v1, v2
        -0.5f,  0.5f, 0.0f, // v0
        -0.5f, -0.5f, 0.0f, // v1
         0.0f,  0.0f, 0.0f, // v2
        // Triangulo 2: v3, v4, v5
         0.0f,  0.0f, 0.0f, // v3
         0.5f, -0.5f, 0.0f, // v4
         0.5f,  0.5f, 0.0f  // v5
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    std::cout << "Pressione 1 (preenchido), 2 (contorno), 3 (pontos), 4 (todos juntos)\n";

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        auto drawFill = [&]() {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUniform4f(colorLoc, 0.2f, 0.4f, 0.9f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        };
        auto drawLine = [&]() {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        };
        auto drawPoints = [&]() {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glPointSize(8.0f);
            glUniform4f(colorLoc, 1.0f, 0.8f, 0.0f, 1.0f);
            glDrawArrays(GL_POINTS, 0, 6);
        };

        if (drawMode == 1) drawFill();
        else if (drawMode == 2) drawLine();
        else if (drawMode == 3) drawPoints();
        else if (drawMode == 4) { drawFill(); drawLine(); drawPoints(); }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}