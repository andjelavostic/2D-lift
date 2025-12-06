#include "Cursor.h"
#include "../2D-lift/Header/Util.h"  // loadImageToTexture ili tvoja funkcija za teksturu

Cursor::Cursor() {
    loadTextures("resources/cursor_black.png","resources/cursor_colored.png");
    // kreiranje VAO/VBO
    glGenVertexArrays(1, &cursorVAO);
    glGenBuffers(1, &cursorVBO);

    glBindVertexArray(cursorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);

    // alociramo prostor za 4 verteksa * 4 floats (x,y,u,v)
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // pozicija
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Cursor::~Cursor() {
    glDeleteBuffers(1, &cursorVBO);
    glDeleteVertexArrays(1, &cursorVAO);
    glDeleteTextures(1, &cursorBlackTex);
    glDeleteTextures(1, &cursorColorTex);
}

void Cursor::loadTextures(const char* blackPath, const char* colorPath) {
    cursorBlackTex = loadImageToTexture(blackPath);
    cursorColorTex = loadImageToTexture(colorPath);
}

void Cursor::draw(GLuint shader, double mouseX, double mouseY, int screenWidth, int screenHeight) {
    glUseProgram(shader);

    // izaberemo teksturu
    GLuint cursorTex = ventilationOn ? cursorColorTex : cursorBlackTex;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cursorTex);
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);

    // mouse -> NDC [-1,1]
    float x = (float)((mouseX / screenWidth) * 2.0 - 1.0);
    float y = (float)(1.0 - (mouseY / screenHeight) * 2.0);

    float size = 0.07f;

    float verts[] = {
        x - size, y - size, 0.0f, 0.0f,
        x + size, y - size, 1.0f, 0.0f,
        x + size, y + size, 1.0f, 1.0f,
        x - size, y + size, 0.0f, 1.0f
    };

    // update buffer
    glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    // draw
    glBindVertexArray(cursorVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}
