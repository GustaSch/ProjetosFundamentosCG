#include "../helpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    GLFWwindow *window = helpers::createWindow(800, 600, "Lista 2 - Ex5: 4 viewports");
    if (!window)
        return -1;
    glfwSetKeyCallback(window, helpers::keyCallback);

    GLuint program = helpers::createProgram();

    const float r = 1.0f, g = 0.0f, b = 1.0f;
    helpers::Mesh triangle = helpers::createMesh({
        400.0f, 150.0f, r, g, b,
        550.0f, 450.0f, r, g, b,
        250.0f, 450.0f, r, g, b,
    });

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, fbWidth, fbHeight);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));

        const int vw = fbWidth / 2, vh = fbHeight / 2;
        for (int row = 0; row < 2; ++row)
        {
            for (int col = 0; col < 2; ++col)
            {
                const int vx = col * vw;
                const int vy = row * vh;

                // Fundo alternado para evidenciar cada quadrante
                const float shade = ((row + col) % 2 == 0) ? 0.92f : 0.97f;
                glEnable(GL_SCISSOR_TEST);
                glScissor(vx, vy, vw, vh);
                glClearColor(shade, shade, shade, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glDisable(GL_SCISSOR_TEST);

                glViewport(vx, vy, vw, vh);
                helpers::drawMesh(triangle);
            }
        }

        glfwSwapBuffers(window);
    }

    helpers::destroyMesh(triangle);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
