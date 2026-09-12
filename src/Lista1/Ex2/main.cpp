#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

const float PI = 3.14159265359f;

const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
uniform vec4 uColor;
void main() { FragColor = uColor; }
)";

int shapeMode = 1; // 1..6

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success; char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(shader, 512, nullptr, infoLog); std::cout << infoLog << std::endl; }
    return shader;
}

// Poligono regular (octagono, pentagono, circulo "grande") via fan, incluindo o centro
std::vector<float> generateRegularPolygon(int sides, float radius = 0.6f) {
    std::vector<float> verts;
    verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(0.0f); // centro
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * PI * i / sides;
        verts.push_back(radius * cos(angle));
        verts.push_back(radius * sin(angle));
        verts.push_back(0.0f);
    }
    return verts;
}

// Pac-man: fan com uma "boca" (gap) aberta
std::vector<float> generatePacman(float gapDeg = 40.0f, int segments = 60, float radius = 0.6f) {
    std::vector<float> verts;
    verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(0.0f);
    float startAngle = gapDeg / 2.0f * PI / 180.0f;
    float endAngle = 2.0f * PI - startAngle;
    for (int i = 0; i <= segments; i++) {
        float angle = startAngle + (endAngle - startAngle) * i / segments;
        verts.push_back(radius * cos(angle));
        verts.push_back(radius * sin(angle));
        verts.push_back(0.0f);
    }
    return verts;
}

// Fatia de pizza: fan cobrindo apenas um pequeno arco
std::vector<float> generatePizzaSlice(float sliceDeg = 50.0f, int segments = 20, float radius = 0.6f) {
    std::vector<float> verts;
    verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(0.0f);
    float half = sliceDeg / 2.0f * PI / 180.0f;
    for (int i = 0; i <= segments; i++) {
        float angle = -half + (2 * half) * i / segments;
        verts.push_back(radius * cos(angle));
        verts.push_back(radius * sin(angle));
        verts.push_back(0.0f);
    }
    return verts;
}

// Estrela: fan alternando raio externo/interno
std::vector<float> generateStar(int points = 5, float outerR = 0.6f, float innerR = 0.25f) {
    std::vector<float> verts;
    verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(0.0f);
    int totalVerts = points * 2;
    for (int i = 0; i <= totalVerts; i++) {
        float angle = PI / 2.0f + 2.0f * PI * i / totalVerts; // comeca apontando pra cima
        float r = (i % 2 == 0) ? outerR : innerR;
        verts.push_back(r * cos(angle));
        verts.push_back(r * sin(angle));
        verts.push_back(0.0f);
    }
    return verts;
}

// Espiral: raio cresce conforme o angulo aumenta (desenhada como line strip)
std::vector<float> generateSpiral(int turns = 4, int pointsPerTurn = 40, float maxRadius = 0.6f) {
    std::vector<float> verts;
    int totalPoints = turns * pointsPerTurn;
    for (int i = 0; i <= totalPoints; i++) {
        float angle = 2.0f * PI * i / pointsPerTurn;
        float radius = maxRadius * ((float)i / totalPoints);
        verts.push_back(radius * cos(angle));
        verts.push_back(radius * sin(angle));
        verts.push_back(0.0f);
    }
    return verts;
}

void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) shapeMode = 1; // octagono
        if (key == GLFW_KEY_2) shapeMode = 2; // pentagono
        if (key == GLFW_KEY_3) shapeMode = 3; // pacman
        if (key == GLFW_KEY_4) shapeMode = 4; // fatia de pizza
        if (key == GLFW_KEY_5) shapeMode = 5; // estrela (desafio 1)
        if (key == GLFW_KEY_6) shapeMode = 6; // espiral (desafio 2)
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Exercicio 2 - Circulo e Derivados", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs); glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint colorLoc = glGetUniformLocation(program, "uColor");

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    std::cout << "1=Octagono 2=Pentagono 3=Pacman 4=Fatia de pizza 5=Estrela 6=Espiral\n";

    int lastMode = -1;
    int vertexCount = 0;
    GLenum primitive = GL_TRIANGLE_FAN;

    while (!glfwWindowShouldClose(window)) {
        if (shapeMode != lastMode) {
            std::vector<float> data;
            primitive = GL_TRIANGLE_FAN;
            switch (shapeMode) {
                case 1: data = generateRegularPolygon(8); break;
                case 2: data = generateRegularPolygon(5); break;
                case 3: data = generatePacman(); break;
                case 4: data = generatePizzaSlice(); break;
                case 5: data = generateStar(); break;
                case 6: data = generateSpiral(); primitive = GL_LINE_STRIP; break;
            }
            vertexCount = (int)(data.size() / 3);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
            lastMode = shapeMode;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform4f(colorLoc, 0.9f, 0.6f, 0.1f, 1.0f);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(primitive, 0, vertexCount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}