#include "Person.h"
#include <GL/glew.h>

Person::Person(int startFloor, float floorSpacing, float w, float heightFactor, float startX) {
    width = w;
    height = floorSpacing * heightFactor;
    floor = startFloor;
    posX = startX;

    x0 = startX;
    x1 = x0 + width;
    y0 = -1.0f + floor * floorSpacing;
    y1 = y0 + height;

    float vertices[] = { x0, y0, x1, y0, x1, y1, x0, y1 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    personSpeed = 0.001f;
}

void Person::draw(GLuint shader) {
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    GLuint offsetLoc = glGetUniformLocation(shader, "uOffset");
    glUniform2f(offsetLoc, posX, 0.0f);
    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.0f, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Person::moveLeft() {
    posX -= personSpeed;
    if (posX < 0.0f) posX = 0.0f;
}

void Person::moveRight(float liftX0) {
    posX += personSpeed;

    float personRight = x0 + posX + width; // apsolutna desna pozicija osobe
    if (personRight > liftX0) {
        posX = liftX0 - width - x0; // postavi posX tako da osoba stane na levu ivicu lifta
    }
}
bool Person::touchesLift(float liftX0)
{
    float personRight = x0 + posX + width;
    return personRight >= liftX0;
}


