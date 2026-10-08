#include "../helpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    GLFWwindow *window = helpers::createWindow(800, 600, "Lista 2 - Ex2: ortho (0, 800, 600, 0)");
    if (!window)
        return -1;
    glfwSetKeyCallback(window, helpers::keyCallback);

    GLuint program = helpers::createProgram();

    // O mesmo triângulo do Ex1, agora em coordenadas de pixel
    const float r = 1.0f, g = 0.0f, b = 1.0f;
    helpers::Mesh triangle = helpers::createMesh({
        400.0f, 150.0f, r, g, b, // topo
        550.0f, 450.0f, r, g, b, // base direita
        250.0f, 450.0f, r, g, b, // base esquerda
    });

    // ortho(left, right, bottom, top): bottom = 600 e top = 0 invertem o eixo y
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        helpers::drawMesh(triangle);

        glfwSwapBuffers(window);
    }

    helpers::destroyMesh(triangle);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
