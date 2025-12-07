#include "../Header/Elevator.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Header/Util.h"
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

    // LiftY0 je offset za shader, odmah na startFloor
    liftY0 = -1.0f + startFloor * floorSpacing;

    float liftVertices[] = {
        liftX0, 0.0f, 0.0f, 0.0f,           // donji-levo
        liftX1, 0.0f, 1.0f, 0.0f,           // donji-desno
        liftX1, liftHeight, 1.0f, 1.0f,    // gornji-desno
        liftX0, liftHeight, 0.0f, 1.0f     // gornji-levo
    };

    glGenVertexArrays(1, &liftVAO);
    glGenBuffers(1, &liftVBO);

    glBindVertexArray(liftVAO);
    glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(liftVertices), liftVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
    glUniform1f(glGetUniformLocation(shader, "uAlpha"), 1.0f);

    for (int i = 0; i < totalFloors; i++) {
        glBindVertexArray(floorVAOs[i]);
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void Elevator::drawLift(GLuint shader) {
    GLuint texToUse = doorsOpen ? texOpened : texClosed;
    glUseProgram(shader);

    // Pomak lifta kroz shader uniform
    glUniform2f(glGetUniformLocation(shader, "uOffset"), 0.0f, liftY0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texToUse);
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shader, "uHighlight"), 0);
    glUniform1f(glGetUniformLocation(shader, "uAlpha"), 1.0f);

    glBindVertexArray(liftVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Elevator::callLift(int floor) {
    float targetY = -1.0f + floor * floorSpacing;

    // Ako je ve? na putu, samo dodaj na kraj
    if (fabs(liftY0 - (-1.0f + liftFloor * floorSpacing)) > 0.001f) {
        if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end())
            targetFloors.push_back(floor);
        return;
    }

    // Ako je lift ve? na tom spratu
    if (fabs(liftY0 - targetY) < 0.001f) {
        if (!doorsOpen) {
            doorsOpen = true;
            doorOpenTime = (float)glfwGetTime();
            doorDuration = 5.0f;
            doorExtended = false;
        }
        return;
    }

    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end())
        targetFloors.push_back(floor);
}

void Elevator::updateLift(PanelGrid& panelGrid, bool personInLift) {
    float now = glfwGetTime();
    auto& buttons = panelGrid.getFloorButtons();

    // 1) Dodaj spratove redom kojim su kliknuti (ako je osoba u liftu)
    if (personInLift) {
        if (buttons[11].pressed) {  // ventilacija
            ventilationOn = true;
            buttons[11].pressed = true;
        }
        for (int i = 0; i < 8; i++) {
            if (buttons[i].floor == liftFloor && doorsOpen && targetFloors.size() == 0) {
                buttons[i].pressed = false;
                buttons[i].highlight = false;
                continue;
            }
            if (buttons[i].pressed && std::find(targetFloors.begin(), targetFloors.end(), buttons[i].floor) == targetFloors.end()) {
                targetFloors.push_back(buttons[i].floor);
                buttons[i].pressed = false;
            }
        }
        if (!doorsOpen) {
            buttons[8].pressed = false;
            buttons[9].pressed = false;
        }

        if (doorsOpen) {
            if (buttons[8].pressed) { // zatvori vrata odmah
                float elapsed = now - doorOpenTime;
                if (elapsed < doorDuration)
                    doorOpenTime = now - doorDuration + 0.05f;
                buttons[8].pressed = false;
            }
            if (buttons[9].pressed && !doorExtended) { // produžena vrata
                float elapsed = now - doorOpenTime;
                doorDuration = elapsed + 5.1f;
                doorExtended = true;
                buttons[9].pressed = false;
            }
        }
    }

    // 2) Ako su vrata otvorena, ?ekaj dok ne istekne trajanje
    if (doorsOpen) {
        if (now - doorOpenTime >= doorDuration) {
            doorsOpen = false;
            lastDoorCloseTime = now;
        }
        else {
            return;
        }
    }

    // 3) Ako nema ciljeva, lift stoji
    if (targetFloors.empty()) return;

    // 4) Pomeri lift ka slede?em spratu
    int nextFloor = targetFloors.front();
    float targetY = -1.0f + nextFloor * floorSpacing;

    if (fabs(liftY0 - targetY) > 0.001f) {
        liftY0 += (targetY > liftY0 ? liftSpeed : -liftSpeed);
    }
    else {
        liftY0 = targetY;
        liftFloor = nextFloor;

        doorsOpen = true;
        doorOpenTime = now;
        doorDuration = 5.0f;
        doorExtended = false;

        if (ventilationOn && !targetFloors.empty() && liftFloor == targetFloors.front()) {
            ventilationOn = false;
            buttons[11].pressed = false;
        }

        for (auto& btn : panelGrid.getFloorButtons()) {
            if (btn.floor == liftFloor) {
                btn.pressed = false;
                btn.highlight = false;
            }
        }

        targetFloors.erase(targetFloors.begin());
    }
}
