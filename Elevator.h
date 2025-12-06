#pragma once
#include <GL/glew.h>
#include <vector>
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
    float doorOpenTime;
    float doorDuration = 5.0f;

    //Teksture
    GLuint texClosed;
    GLuint texOpened;

public:
    Elevator(int floors = 9, int startFloor = 2, float x0 = 0.75f, float width = 0.15f);

    void drawFloors(GLuint shader);
    void drawLift(GLuint shader);

    void callLift(int floor);         // Poziva lift na odredjeni sprat
    void updateLift();

    float getFloorSpacing() { return floorSpacing; }
    int getLiftFloor() { return liftFloor; }
    float getLiftX0() { return liftX0; }
    float getLiftX1() { return liftX1; }
    float getLiftY0() { return liftY0; }
    bool isDoorsOpen() { return doorsOpen; }
};
