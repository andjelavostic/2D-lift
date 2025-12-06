#pragma once
#include <GL/glew.h>
#include <vector>
#include "PanelGrid.h"
using namespace std;

class Elevator {
private:
    int totalFloors;
    float floorSpacing;

    vector<GLuint> floorVAOs;
    vector<GLuint> floorVBOs;

    GLuint liftVAO, liftVBO;
    float liftX0, liftX1, liftY0, liftY1;
    float liftWidth, liftHeight;
    int liftFloor;
    float liftSpeed;

    // Stanje lifta
    bool liftCalled;
    int targetFloor;
    bool doorsOpen;
    float doorDuration = 5.0f;
    std::vector<int> targetFloors; // spratovi koje lift treba da obi?e
    bool movingUp = false;
    float doorOpenTime = 0.0f;         // broji sekunde otvaranja vrata
    double lastDoorCloseTime = 0.0; // pamti kada su vrata zatvorena

    //Teksture
    GLuint texClosed;
    GLuint texOpened;

public:
    Elevator(int floors = 9, int startFloor = 2, float x0 = 0.75f, float width = 0.15f);

    void drawFloors(GLuint shader);
    void drawLift(GLuint shader);

    void callLift(int floor);         // Poziva lift na odredjeni sprat
    void updateLift(PanelGrid& panelGrid, bool personInLift);

    float getFloorSpacing() { return floorSpacing; }
    int getLiftFloor() { return liftFloor; }
    float getLiftX0() { return liftX0; }
    float getLiftX1() { return liftX1; }
    float getLiftY0() { return liftY0; }
    float getLiftY1() { return liftY1; }
    bool isDoorsOpen() { return doorsOpen; }
};
