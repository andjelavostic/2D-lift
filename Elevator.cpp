#include "Elevator.h"
#include <GL/glew.h>
#include <cmath>
#include <GLFW/glfw3.h>

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

    float liftVertices[] = { liftX0, liftY0, liftX1, liftY0, liftX1, liftY1, liftX0, liftY1 };
    glGenVertexArrays(1, &liftVAO);
    glGenBuffers(1, &liftVBO);
    glBindVertexArray(liftVAO);
    glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(liftVertices), liftVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    liftCalled = false;
    targetFloor = startFloor;
    doorsOpen = false;
    doorOpenTime = 0.0f;
    liftSpeed = 0.005f;
}

void Elevator::drawFloors(GLuint shader) {
    glUniform3f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < totalFloors; i++) {
        glBindVertexArray(floorVAOs[i]);
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void Elevator::drawLift(GLuint shader, bool doorsStatus) {
    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.8f, 0.3f, 0.2f);
    glBindVertexArray(liftVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    doorsOpen = doorsStatus;
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

            float liftVertices[] = { liftX0, liftY0, liftX1, liftY0, liftX1, liftY1, liftX0, liftY1 };
            glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(liftVertices), liftVertices);
        }
        else {
            liftY0 = targetY;
            liftY1 = liftY0 + liftHeight;
            liftFloor = targetFloor;
            liftCalled = false;
            doorsOpen = true;
            doorOpenTime = glfwGetTime();
        }
    }

    if (doorsOpen && (glfwGetTime() - doorOpenTime > doorDuration)) {
        doorsOpen = false;
    }
}