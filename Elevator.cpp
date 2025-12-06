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
    // Ako je lift na tom spratu i vrata nisu otvorena
    if (floor == liftFloor) {
        if (!doorsOpen) {
            doorsOpen = true;
            doorOpenTime = (float)glfwGetTime();
            doorDuration = 5.0f;
            doorExtended = false;
        }
        // Ako su vrata ve? otvorena, ne diraj ništa
        return; // ne dodaj u targetFloors
    }

    // Ako sprat još nije u listi ciljeva
    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end()) {
        targetFloors.push_back(floor);
    }
}
void Elevator::updateLift(PanelGrid& panelGrid, bool personInLift)
{
    float now = glfwGetTime();

    // 1) Dodaj spratove redom kojim su kliknuti (ako je osoba u liftu)
    if (personInLift)
    {
        auto& buttons = panelGrid.getFloorButtons();
        // Dugme ventilacije je indeks 11
        if (buttons[11].pressed) {
            ventilationOn = true;       // uklju?i ventilaciju
            buttons[11].pressed = false; // reset dugmeta
        }
        for (int i = 0; i < 8; i++) // indeksi 0..7 su spratovi
        {
            if (buttons[i].floor == liftFloor && doorsOpen && targetFloors.size() == 0) {
                // Ne produžavamo duration, samo reset dugmeta i highlight
                buttons[i].pressed = false;
                buttons[i].highlight = false;
                continue;
            }
            if (buttons[i].pressed &&
                std::find(targetFloors.begin(), targetFloors.end(), buttons[i].floor) == targetFloors.end())
            {
                targetFloors.push_back(buttons[i].floor);
                buttons[i].pressed = false; // reset dugmeta
            }
        }

        // Dugmad za vrata dok su otvorena (produženje)
        if (doorsOpen)
        {
            if (buttons[8].pressed) // zatvori vrata odmah
            {
                float elapsed = now - doorOpenTime;
                if (elapsed < doorDuration)
                    doorOpenTime = now - doorDuration + 0.05f; // kratki offset
                buttons[8].pressed = false;
            }

            // Dugme za produženje vrata (indeks 9)
            if (buttons[9].pressed && !doorExtended)
            {
                float elapsed = now - doorOpenTime;          // koliko je vrata ve? otvoreno
                doorDuration = elapsed + 5.0f+0.1f;  // produžimo za 5 sekundi ukupno
                doorExtended = true;                         // može se samo jednom
                buttons[9].pressed = false;
            }


        }
    }

    // 2) Ako su vrata otvorena, ?ekaj dok ne istekne trajanje
    if (doorsOpen)
    {
        if (now - doorOpenTime >= doorDuration)
        {
            doorsOpen = false;
            lastDoorCloseTime = now;
        }
        else
        {
            return; // lift se ne pomera dok su vrata otvorena
        }
    }

    // 3) Ako nema ciljeva, lift stoji
    if (targetFloors.empty()) return;

    // 4) Pomeri lift ka slede?em spratu
    int nextFloor = targetFloors.front();
    float targetY = -1.0f + nextFloor * floorSpacing;

    if (fabs(liftY0 - targetY) > 0.001f)
    {
        // pomeri lift
        liftY0 += (targetY > liftY0 ? liftSpeed : -liftSpeed);
        liftY1 = liftY0 + liftHeight;

        // update vertex buffera
        float liftVertices[] = {
            liftX0, liftY0, 0.0f, 0.0f,
            liftX1, liftY0, 1.0f, 0.0f,
            liftX1, liftY1, 1.0f, 1.0f,
            liftX0, liftY1, 0.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(liftVertices), liftVertices);
    }
    else
    {
        // Lift je stigao na sprat
        liftY0 = targetY;
        liftY1 = liftY0 + liftHeight;
        liftFloor = nextFloor;

        // Otvori vrata uvek na 5 sekundi
        doorsOpen = true;
        doorOpenTime = now;
        doorDuration = 5.0f;
        doorExtended = false;
        // Ako je lift stigao na sprat i ventilacija je uklju?ena, a ovo je prvi cilj
        if (ventilationOn && !targetFloors.empty() && liftFloor == targetFloors.front()) {
            ventilationOn = false;
        }
        // reset dugme i highlight
        for (auto& btn : panelGrid.getFloorButtons())
        {
            if (btn.floor == liftFloor)
            {
                btn.pressed = false;
                btn.highlight = false;
            }
        }

        // ukloni sprat iz liste ciljeva
        targetFloors.erase(targetFloors.begin());
    }
}
