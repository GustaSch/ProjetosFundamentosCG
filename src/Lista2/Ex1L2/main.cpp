#include "../helpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    GLFWwindow *window = helpers::createWindow(800, 600, "Lista 2 - Ex1: ortho [-10, 10]");
    if (!window)
        return -1;
    glfwSetKeyCallback(window, helpers::keyCallback);

    GLuint program = helpers::createProgram();

    // Triângulo magenta descrito nas coordenadas do mundo (-10..10)
    const float r = 1.0f, g = 0.0f, b = 1.0f;
    helpers::Mesh triangle = helpers::createMesh({
        -5.0f, -5.0f, r, g, b,
         5.0f, -5.0f, r, g, b,
         0.0f,  5.0f, r, g, b,
    });

    // Janela do mundo: xmin, xmax, ymin, ymax
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);

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
