#include "Panel.h"
#include <GL/glew.h>

Panel::Panel(float x0, float x1, float y0, float y1, float r, float g, float b) {
    // Cuvamo boju
    color[0] = r; color[1] = g; color[2] = b;

    // Inicijalizujemo temena
    vertices[0] = x0; vertices[1] = y1;
    vertices[2] = x1; vertices[3] = y1;
    vertices[4] = x1; vertices[5] = y0;
    vertices[6] = x0; vertices[7] = y0;

    // Kreiramo VAO i VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Panel::draw(GLuint shader) {
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    glUniform3f(glGetUniformLocation(shader, "uColor"), color[0], color[1], color[2]);
    glUniform2f(glGetUniformLocation(shader, "uOffset"), 0.0f, 0.0f);
    glUniform1i(glGetUniformLocation(shader, "uHighlight"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

Panel::~Panel() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
