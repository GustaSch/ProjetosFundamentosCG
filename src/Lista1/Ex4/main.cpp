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

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success; char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(shader, 512, nullptr, infoLog); std::cout << infoLog << std::endl; }
    return shader;
}

// Gera um circulo/elipse preenchido (triangle fan) via equacao parametrica
std::vector<float> generateEllipse(float cx, float cy, float rx, float ry, int segments = 60) {
    std::vector<float> v;
    v.push_back(cx); v.push_back(cy); v.push_back(0.0f); // centro
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        v.push_back(cx + rx * cos(angle));
        v.push_back(cy + ry * sin(angle));
        v.push_back(0.0f);
    }
    return v;
}

GLuint uploadVAO(const std::vector<float>& data) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return VAO;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Exercicio 4 - Rosto de Gato", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs); glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint colorLoc = glGetUniformLocation(program, "uColor");

    // ----- Cabeca (elipse creme) -----
    auto head = generateEllipse(0.0f, -0.05f, 0.55f, 0.5f);

    // ----- Orelhas (triangulos) -----
    float earL[] = {
        -0.55f, 0.30f, 0.0f,
        -0.15f, 0.35f, 0.0f,
        -0.40f, 0.80f, 0.0f
    };
    float earR[] = {
         0.55f, 0.30f, 0.0f,
         0.15f, 0.35f, 0.0f,
         0.40f, 0.80f, 0.0f
    };
    // Interior rosa das orelhas (triangulo menor)
    float earInnerL[] = {
        -0.47f, 0.35f, 0.0f,
        -0.24f, 0.38f, 0.0f,
        -0.38f, 0.65f, 0.0f
    };
    float earInnerR[] = {
         0.47f, 0.35f, 0.0f,
         0.24f, 0.38f, 0.0f,
         0.38f, 0.65f, 0.0f
    };

    // ----- Olhos (circulos pretos) -----
    auto eyeL = generateEllipse(-0.22f, -0.05f, 0.08f, 0.09f);
    auto eyeR = generateEllipse( 0.22f, -0.05f, 0.08f, 0.09f);
    // Brilhinho branco no olho (opcional)
    auto eyeShineL = generateEllipse(-0.25f, -0.01f, 0.02f, 0.02f);
    auto eyeShineR = generateEllipse( 0.19f, -0.01f, 0.02f, 0.02f);

    // ----- Bochechas (circulos rosados) -----
    auto cheekL = generateEllipse(-0.32f, -0.30f, 0.10f, 0.06f);
    auto cheekR = generateEllipse( 0.32f, -0.30f, 0.10f, 0.06f);

    // ----- Nariz (pequeno triangulo) -----
    float nose[] = {
        -0.03f, -0.20f, 0.0f,
         0.03f, -0.20f, 0.0f,
         0.00f, -0.24f, 0.0f
    };

    // ----- Boca ("w" feito de linhas) -----
// ----- Boca (linha em "W", simetrica, contornando o nariz) -----
float mouth[] = {
     0.11f, -0.26f, 0.0f,
     0.05f, -0.30f, 0.0f, // vale direito
     0.00f, -0.24f, 0.0f,
    -0.05f, -0.30f, 0.0f,
    -0.11f, -0.26f, 0.0f 
};

    // ----- Bigodes (linhas) -----
    float whiskersL[] = {
        -0.45f, -0.18f, 0.0f,  -0.80f, -0.22f, 0.0f,
        -0.45f, -0.24f, 0.0f,  -0.80f, -0.24f, 0.0f,
        -0.45f, -0.30f, 0.0f,  -0.80f, -0.26f, 0.0f
    };
    float whiskersR[] = {
        0.45f, -0.18f, 0.0f,  0.80f, -0.22f, 0.0f,
        0.45f, -0.24f, 0.0f,  0.80f, -0.24f, 0.0f,
        0.45f, -0.30f, 0.0f,  0.80f, -0.26f, 0.0f
    };

    GLuint headVAO       = uploadVAO(head);
    GLuint earLVAO       = uploadVAO(std::vector<float>(earL, earL+9));
    GLuint earRVAO       = uploadVAO(std::vector<float>(earR, earR+9));
    GLuint earInnerLVAO  = uploadVAO(std::vector<float>(earInnerL, earInnerL+9));
    GLuint earInnerRVAO  = uploadVAO(std::vector<float>(earInnerR, earInnerR+9));
    GLuint eyeLVAO       = uploadVAO(eyeL);
    GLuint eyeRVAO       = uploadVAO(eyeR);
    GLuint eyeShineLVAO  = uploadVAO(eyeShineL);
    GLuint eyeShineRVAO  = uploadVAO(eyeShineR);
    GLuint cheekLVAO     = uploadVAO(cheekL);
    GLuint cheekRVAO     = uploadVAO(cheekR);
    GLuint noseVAO       = uploadVAO(std::vector<float>(nose, nose+9));
    GLuint mouthVAO      = uploadVAO(std::vector<float>(mouth, mouth+15));
    GLuint whiskersLVAO  = uploadVAO(std::vector<float>(whiskersL, whiskersL+18));
    GLuint whiskersRVAO  = uploadVAO(std::vector<float>(whiskersR, whiskersR+18));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        auto drawFan = [&](GLuint vao, int count, float r, float g, float b) {
            glUniform4f(colorLoc, r, g, b, 1.0f);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, count);
        };
        auto drawTri = [&](GLuint vao, float r, float g, float b) {
            glUniform4f(colorLoc, r, g, b, 1.0f);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        };
        auto drawLines = [&](GLuint vao, int count, float r, float g, float b) {
            glUniform4f(colorLoc, r, g, b, 1.0f);
            glBindVertexArray(vao);
            glDrawArrays(GL_LINES, 0, count);
        };

        // Orelhas (atras da cabeca)
        drawTri(earLVAO, 0.05f, 0.05f, 0.05f);
        drawTri(earRVAO, 0.05f, 0.05f, 0.05f);

        // Cabeca (creme)
        drawFan(headVAO, (int)(head.size()/3), 0.99f, 0.96f, 0.87f);

        // Interior das orelhas (rosa)
        drawTri(earInnerLVAO, 0.95f, 0.75f, 0.72f);
        drawTri(earInnerRVAO, 0.95f, 0.75f, 0.72f);

        // Bochechas
        drawFan(cheekLVAO, (int)(cheekL.size()/3), 0.97f, 0.75f, 0.70f);
        drawFan(cheekRVAO, (int)(cheekR.size()/3), 0.97f, 0.75f, 0.70f);

        // Olhos
        drawFan(eyeLVAO, (int)(eyeL.size()/3), 0.05f, 0.05f, 0.05f);
        drawFan(eyeRVAO, (int)(eyeR.size()/3), 0.05f, 0.05f, 0.05f);
        drawFan(eyeShineLVAO, (int)(eyeShineL.size()/3), 1.0f, 1.0f, 1.0f);
        drawFan(eyeShineRVAO, (int)(eyeShineR.size()/3), 1.0f, 1.0f, 1.0f);

        // Nariz
        drawTri(noseVAO, 0.05f, 0.05f, 0.05f);

        // Boca e bigodes (linhas)
        // Trocar GL_LINES por GL_LINE_STRIP, e o count de 6 para 5
        glLineWidth(2.0f);
        glUniform4f(colorLoc, 0.05f, 0.05f, 0.05f, 1.0f);
        glBindVertexArray(mouthVAO);
        glDrawArrays(GL_LINE_STRIP, 0, 5);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}