#include "Elevator.h"
#include <GL/glew.h>
#include <cmath>
#include <GLFW/glfw3.h>
#include "../2D-lift/Header/Util.h"
#include <iostream>
#include <algorithm>
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
    // Dodaj sprat ako ve? nije u listi
    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end()) {
        targetFloors.push_back(floor);
    }

    // Sortiraj listu po trenutnom pravcu
    if (liftFloor <= floor) {
        movingUp = true;
        std::sort(targetFloors.begin(), targetFloors.end()); // rastu?e
    }
    else {
        movingUp = false;
        std::sort(targetFloors.rbegin(), targetFloors.rend()); // opadaju?e
    }
}

void Elevator::updateLift(PanelGrid& panelGrid, bool personInLift) {
    float currentTime = glfwGetTime();

    // Dodaj kliknute spratove samo kada je osoba unutra i vrata zatvorena
    if (personInLift && !doorsOpen && currentTime - lastDoorCloseTime > 1.0f) {
        for (auto& b : panelGrid.getFloorButtons()) {
            if (b.pressed && std::find(targetFloors.begin(), targetFloors.end(), b.floor) == targetFloors.end()) {
                targetFloors.push_back(b.floor);
            }
        }

        // Podesi redosled po trenutnom pravcu
        if (!targetFloors.empty()) {
            std::vector<int> upFloors, downFloors;
            for (int f : targetFloors) {
                if (f >= liftFloor) upFloors.push_back(f);
                else downFloors.push_back(f);
            }

            // Ako lift ide gore ili je na mestu i ima više spratova iznad
            if (!upFloors.empty()) {
                std::sort(upFloors.begin(), upFloors.end());
                targetFloors = upFloors;
                targetFloors.insert(targetFloors.end(), downFloors.begin(), downFloors.end());
            }
            else {
                std::sort(downFloors.begin(), downFloors.end(), std::greater<int>());
                targetFloors = downFloors;
            }
        }
    }

    // Ako još ?ekamo 1 sekundu nakon zatvaranja vrata
    if (!doorsOpen && (currentTime - lastDoorCloseTime < 1.0))
        return;

    // Pomeri lift ka slede?em spratu
    if (!targetFloors.empty()) {
        int nextFloor = targetFloors.front();
        float targetY = -1.0f + nextFloor * floorSpacing;

        if (std::abs(liftY0 - targetY) > 0.001f) {
            liftY0 += (targetY > liftY0 ? liftSpeed : -liftSpeed);
            liftY1 = liftY0 + liftHeight;

            float liftVertices[] = {
                liftX0, liftY0, 0.0f, 0.0f,
                liftX1, liftY0, 1.0f, 0.0f,
                liftX1, liftY1, 1.0f, 1.0f,
                liftX0, liftY1, 0.0f, 1.0f
            };
            glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(liftVertices), liftVertices);
        }
        else {
            // lift stigao
            liftY0 = targetY;
            liftY1 = liftY0 + liftHeight;
            liftFloor = nextFloor;

            doorsOpen = true;
            doorOpenTime = currentTime;

            // reset dugme
            for (auto& b : panelGrid.getFloorButtons()) {
                if (b.floor == liftFloor) {
                    b.pressed = false;
                    b.highlight = false;
                    break;
                }
            }

            // ukloni sprat iz liste
            targetFloors.erase(targetFloors.begin());
        }
    }

    // Zatvori vrata nakon 5 sekundi
    if (doorsOpen && (currentTime - doorOpenTime > 5.0f)) {
        doorsOpen = false;
        lastDoorCloseTime = currentTime;
    }
}
