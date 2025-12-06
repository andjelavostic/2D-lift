#include "Elevator.h"
#include <GL/glew.h>
#include <cmath>
#include <GLFW/glfw3.h>
#include "../2D-lift/Header/Util.h"
#include <iostream>
Elevator::Elevator(int floors, int startFloor, float x0, float width) {
    totalFloors = floors;
    floorSpacing = 2.0f / (totalFloors - 1);

    // Spratovi
    floorVAOs.resize(totalFloors);
    floorVBOs.resize(totalFloors);
    glGenVertexArrays(totalFloors, floorVAOs.data());
    glGenBuffers(totalFloors, floorVBOs.data());

    for (int i = 0; i < totalFloors; i++) {
        float y = -1.0f + i * floorSpacing;
        float line[] = { 0.0f, y, 1.0f, y };
        glBindVertexArray(floorVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // Lift
    liftFloor = startFloor;
    liftWidth = width;
    liftHeight = floorSpacing;
    liftX0 = x0;
    liftX1 = liftX0 + liftWidth;
    liftY0 = -1.0f + startFloor * floorSpacing;
    liftY1 = liftY0 + liftHeight;

    float liftVertices[] = {
        // x, y, u, v
        liftX0, liftY0, 0.0f, 0.0f, // donji-levo
        liftX1, liftY0, 1.0f, 0.0f, // donji-desno
        liftX1, liftY1, 1.0f, 1.0f, // gornji-desno
        liftX0, liftY1, 0.0f, 1.0f  // gornji-levo
    };

    glGenVertexArrays(1, &liftVAO);
    glGenBuffers(1, &liftVBO);

    glBindVertexArray(liftVAO);
    glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(liftVertices), liftVertices, GL_DYNAMIC_DRAW);

    // pozicija
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    liftCalled = false;
    targetFloor = startFloor;
    doorsOpen = false;
    doorOpenTime = 0.0f;
    liftSpeed = 0.005f;

    texClosed = loadImageToTexture("resources/closed.png");
    texOpened = loadImageToTexture("resources/opened.png");
}

void Elevator::drawFloors(GLuint shader) {
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "uHighlight"), 0);
    glUniform3f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < totalFloors; i++) {
        glBindVertexArray(floorVAOs[i]);
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void Elevator::drawLift(GLuint shader) {
    GLuint texToUse = doorsOpen ? texOpened : texClosed;
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texToUse);
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shader, "uHighlight"), 0);

    glBindVertexArray(liftVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}
void Elevator::callLift(int floor) {
    targetFloor = floor;
    liftCalled = true;
}

void Elevator::updateLift() {
    if (liftCalled) {
        float targetY = -1.0f + targetFloor * floorSpacing;
        if (std::abs(liftY0 - targetY) > 0.001f) {
            liftY0 += (targetY > liftY0 ? liftSpeed : -liftSpeed);
            liftY1 = liftY0 + liftHeight;

            // Ponovo kreiramo vertikale sa UV koordinatama
            float liftVertices[] = {
                liftX0, liftY0, 0.0f, 0.0f, // bottom-left
                liftX1, liftY0, 1.0f, 0.0f, // bottom-right
                liftX1, liftY1, 1.0f, 1.0f, // top-right
                liftX0, liftY1, 0.0f, 1.0f  // top-left
            };

            glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(liftVertices), liftVertices);

        }
        else {
            liftY0 = targetY;
            liftY1 = liftY0 + liftHeight;
            liftFloor = targetFloor;
            liftCalled = false;

            doorsOpen = true;                  // otvaranje vrata
            doorOpenTime = glfwGetTime();      // zapamti vreme otvaranja
        }
    }

    // Zatvori vrata nakon 5 sekundi
    if (doorsOpen && (glfwGetTime() - doorOpenTime > 5.0f)) {
        doorsOpen = false;
    }
}
