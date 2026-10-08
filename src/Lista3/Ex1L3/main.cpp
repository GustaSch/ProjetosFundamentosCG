#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------- shaders
static const char *kVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uProjection;

out vec3 vColor;

void main()
{
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

static const char *kFragmentShader = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
)";

static GLuint compileShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Erro ao compilar shader:\n" << log << std::endl;
    }
    return shader;
}

static GLuint createProgram()
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Erro ao linkar programa:\n" << log << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

// ------------------------------------------------------------------ cores
// Converte HSV -> RGB (h, s, v em [0, 1])
static glm::vec3 hsvToRgb(float h, float s, float v)
{
    const float c = v * s;
    const float hp = std::fmod(h, 1.0f) * 6.0f;
    const float x = c * (1.0f - std::fabs(std::fmod(hp, 2.0f) - 1.0f));
    glm::vec3 rgb(0.0f);
    if (hp < 1.0f)      rgb = {c, x, 0.0f};
    else if (hp < 2.0f) rgb = {x, c, 0.0f};
    else if (hp < 3.0f) rgb = {0.0f, c, x};
    else if (hp < 4.0f) rgb = {0.0f, x, c};
    else if (hp < 5.0f) rgb = {x, 0.0f, c};
    else                rgb = {c, 0.0f, x};
    return rgb + glm::vec3(v - c);
}

// ----------------------------------------------------------------- estado
struct App
{
    GLuint vao = 0;
    GLuint vbo = 0;

    // Vértices intercalados: x, y, r, g, b
    std::vector<float> data;
    size_t vertexCount = 0;

    // Gerador de cores: avança o matiz pelo "ângulo áureo" para que cada
    // triângulo receba uma cor visivelmente diferente da anterior.
    float hue = 0.0f;
    glm::vec3 currentColor{1.0f, 0.0f, 0.0f};

    bool dirty = false;
};

static glm::vec3 nextColor(App &app)
{
    app.hue = std::fmod(app.hue + 0.61803398875f, 1.0f);
    return hsvToRgb(app.hue, 0.85f, 0.95f);
}

static void updateTitle(GLFWwindow *window, const App &app)
{
    const std::string title = "Lista 3 - Vertices criados: " + std::to_string(app.vertexCount) +
                              " | Triangulos: " + std::to_string(app.vertexCount / 3);
    glfwSetWindowTitle(window, title.c_str());
}

static void addVertex(GLFWwindow *window, App &app, float x, float y)
{
    // O 1º vértice de cada grupo de 3 define a cor do novo triângulo
    if (app.vertexCount % 3 == 0)
        app.currentColor = nextColor(app);

    app.data.insert(app.data.end(), {x, y, app.currentColor.r, app.currentColor.g, app.currentColor.b});
    ++app.vertexCount;
    app.dirty = true;

    std::cout << "Vertice " << app.vertexCount << " em (" << x << ", " << y << ")";
    if (app.vertexCount % 3 == 0)
        std::cout << "  -> triangulo " << app.vertexCount / 3 << " criado";
    std::cout << std::endl;

    updateTitle(window, app);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int /*mods*/)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS)
        return;

    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));

    // Posição do cursor em coordenadas da janela (origem no canto superior esquerdo).
    // Como a janela do mundo é (0, largura, altura, 0), é a própria coordenada do mundo.
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    addVertex(window, *app, static_cast<float>(xpos), static_cast<float>(ypos));
}

static void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_C)
    {
        App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
        app->data.clear();
        app->vertexCount = 0;
        app->dirty = true;
        updateTitle(window, *app);
    }
}

// ------------------------------------------------------------------- main
int main()
{
    if (!glfwInit())
    {
        std::cerr << "Falha ao inicializar a GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(800, 600, "Lista 3 - Vertices criados: 0 | Triangulos: 0", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar a janela" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Falha ao inicializar a GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    App app;
    glfwSetWindowUserPointer(window, &app);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    GLuint program = createProgram();
    const GLint projLoc = glGetUniformLocation(program, "uProjection");

    // VAO/VBO únicos, com o formato (x, y, r, g, b) por vértice
    glGenVertexArrays(1, &app.vao);
    glGenBuffers(1, &app.vbo);
    glBindVertexArray(app.vao);
    glBindBuffer(GL_ARRAY_BUFFER, app.vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glPointSize(8.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Tamanho da janela (coordenadas de tela, mesmas do cursor) e do framebuffer (pixels reais)
        int winWidth, winHeight, fbWidth, fbHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        if (winWidth == 0 || winHeight == 0)
        {
            glfwSwapBuffers(window);
            continue; // janela minimizada
        }

        glViewport(0, 0, fbWidth, fbHeight);

        // Janela do mundo = tamanho da janela: xmin = 0, xmax = largura, ymin = altura, ymax = 0
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(winWidth), static_cast<float>(winHeight), 0.0f, -1.0f, 1.0f);

        if (app.dirty)
        {
            glBindBuffer(GL_ARRAY_BUFFER, app.vbo);
            glBufferData(GL_ARRAY_BUFFER, app.data.size() * sizeof(float), app.data.data(), GL_DYNAMIC_DRAW);
            app.dirty = false;
        }

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(app.vao);

        // Triângulos completos (múltiplo de 3 vértices)
        const GLsizei triangleVertices = static_cast<GLsizei>((app.vertexCount / 3) * 3);
        if (triangleVertices > 0)
            glDrawArrays(GL_TRIANGLES, 0, triangleVertices);

        // Vértices pendentes (ainda sem formar triângulo) aparecem como pontos
        const GLsizei pending = static_cast<GLsizei>(app.vertexCount % 3);
        if (pending > 0)
            glDrawArrays(GL_POINTS, triangleVertices, pending);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &app.vao);
    glDeleteBuffers(1, &app.vbo);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
