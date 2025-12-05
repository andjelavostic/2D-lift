#include "PanelGrid.h"
#include <iostream>

PanelGrid::PanelGrid(float left, float right, float bottom, float top,
    int r, int c, float buttonWidth, float buttonHeight,
    float hSpacing, float vSpacing,
    float red, float green, float blue)
    : rows(r), cols(c)
{
    color[0] = red; color[1] = green; color[2] = blue;

    int totalButtons = rows * cols;
    VAOs.resize(totalButtons);
    VBOs.resize(totalButtons);
    glGenVertexArrays(totalButtons, VAOs.data());
    glGenBuffers(totalButtons, VBOs.data());

    float totalWidth = cols * buttonWidth + (cols - 1) * hSpacing;
    float totalHeight = rows * buttonHeight + (rows - 1) * vSpacing;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;

            float x0 = left + (right - left - totalWidth) / 2 + j * (buttonWidth + hSpacing);
            float x1 = x0 + buttonWidth;

            float y0 = bottom + (top - bottom - totalHeight) / 2 + (rows - 1 - i) * (buttonHeight + vSpacing);
            float y1 = y0 + buttonHeight;

            float vertices[] = { x0, y0, x1, y0, x1, y1, x0, y1 };

            glBindVertexArray(VAOs[idx]);
            glBindBuffer(GL_ARRAY_BUFFER, VBOs[idx]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
    }

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PanelGrid::draw(GLuint shader) {
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    glUniform3f(glGetUniformLocation(shader, "uColor"), color[0], color[1], color[2]);
    glUniform2f(glGetUniformLocation(shader, "uOffset"), 0.0f, 0.0f);
    for (auto vao : VAOs) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
}

PanelGrid::~PanelGrid() {
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs.size(), VBOs.data());
}
