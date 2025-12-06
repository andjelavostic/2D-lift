#include "Overlay.h"
#include <iostream>

Overlay::Overlay()
    : textureID(0), vao(0), vbo(0), posX(-1.0f), posY(1.0f), width(0.3f), height(0.15f) // malo manje i u uglu
{
    textureID = loadImageToTexture("resources/template.png"); 

    // inicijalizuj VAO/VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // alociramo mesto za 4 verteksa (x,y,u,v)
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // pozicija (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Overlay::~Overlay()
{
    glDeleteTextures(1, &textureID);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
void Overlay::setPosition(float x, float y)
{
    posX = x;
    posY = y;
}

void Overlay::setSize(float w, float h)
{
    width = w;
    height = h;
}

void Overlay::draw(GLuint shader)
{
    glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // uniformi za shader
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
    glUniform2f(glGetUniformLocation(shader, "uOffset"), 0.0f, 0.0f);
    glUniform1i(glGetUniformLocation(shader, "uHighlight"), 0);

    float vertices[] = {
        posX, posY - height, 0.0f, 0.0f,          // donji-levo
        posX + width, posY - height, 1.0f, 0.0f,  // donji-desno
        posX + width, posY, 1.0f, 1.0f,           // gornji-desno
        posX, posY, 0.0f, 1.0f                    // gornji-levo
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}
