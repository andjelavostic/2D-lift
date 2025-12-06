#include "PanelGrid.h"
#include <iostream>
#include "../2D-lift//Header/Util.h"
PanelGrid::PanelGrid(float left, float right, float bottom, float top,
    int r, int c, float buttonWidth, float buttonHeight,
    float hSpacing, float vSpacing,
    float red, float green, float blue)
    : rows(r), cols(c)
{
    color[0] = red; color[1] = green; color[2] = blue;

    int totalButtons = rows * cols;
    floorButtons.push_back({ 0, false, false }); // SU
    floorButtons.push_back({ 1, false, false }); // PR
    for (int f = 2; f <= 7; ++f) {             // 1..6
        floorButtons.push_back({ f, false, false });
    }

    float totalWidth = cols * buttonWidth + (cols - 1) * hSpacing;
    float totalHeight = rows * buttonHeight + (rows - 1) * vSpacing;
    buttons.resize(totalButtons);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;

            float x0 = left + (right - left - totalWidth) / 2 + j * (buttonWidth + hSpacing);
            float x1 = x0 + buttonWidth;

            float y0 = bottom + (top - bottom - totalHeight) / 2 + (rows - 1 - i) * (buttonHeight + vSpacing);
            float y1 = y0 + buttonHeight;

            buttons[idx].texture = loadImageToTexture(texturePaths[idx].c_str());
            buttons[idx].x0 = x0;
            buttons[idx].x1 = x1;
            buttons[idx].y0 = y0;
            buttons[idx].y1 = y1;

            float vertices[] = {
                // x, y, u, v
                x0, y0, 0.0f, 0.0f,
                x1, y0, 1.0f, 0.0f,
                x1, y1, 1.0f, 1.0f,
                x0, y1, 0.0f, 1.0f
            };

            glGenVertexArrays(1, &buttons[idx].VAO);
            glGenBuffers(1, &buttons[idx].VBO);

            glBindVertexArray(buttons[idx].VAO);
            glBindBuffer(GL_ARRAY_BUFFER, buttons[idx].VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // pozicija
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // UV
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
    }
    floorButtons.push_back({ 0, false, false }); // SU
    floorButtons.push_back({ 1, false, false }); // PR
    for (int f = 2; f <= 7; ++f) {             // 1..6
        floorButtons.push_back({ f, false, false });
    }

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PanelGrid::draw(GLuint shader) {
    glUseProgram(shader);

    for (int i = 0; i < buttons.size(); i++) {
        ButtonGraphic& b = buttons[i];
        ButtonState& state = floorButtons[i];

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, b.texture);
        glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);

        // highlight uniform
        glUniform1i(glGetUniformLocation(shader, "uHighlight"), state.highlight ? 1 : 0);
        glUniform1f(glGetUniformLocation(shader, "highlightWidth"), 0.05f);

        glBindVertexArray(b.VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}
void PanelGrid::checkClick(float mouseX, float mouseY, bool inLift) {
    if (!inLift) return; // ne dozvoljavaj klik ako osoba nije u liftu

    // prolazimo kroz sva dugmad
    for (int i = 0; i < buttons.size(); i++) {
        ButtonGraphic& b = buttons[i];
        ButtonState& state = floorButtons[i];

        // provera da li je klik unutar pravougaonika dugmeta
        if (mouseX >= b.x0 && mouseX <= b.x1 &&
            mouseY >= b.y0 && mouseY <= b.y1) {
            if (i >= 0 && i <= 7) {
                state.highlight = true;
            }// dugme se uokviruje / highlight-uje
            state.pressed = true;   // opcionalno: dugme je pritisnuto
            std::cout << "Dugme " << i << " kliknuto!" << std::endl;
        }
    }
}

PanelGrid::~PanelGrid() {
}
