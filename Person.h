#pragma once
#include <GL/glew.h>

class Person {
private:
    GLuint VAO, VBO;
    float x0, x1, y0, y1;
    float width, height;
    float posX;
    int floor;
    float personSpeed;
    unsigned texPerson;      // tekstura osobe
    bool facingRight = true;
    float u0 = 0.0f, u1 = 1.0f; // UV koordinate po X, za flip
    bool isInsideLift = false;

public:
    Person(int startFloor, float floorSpacing, float width = 0.1f, float heightFactor = 0.7f, float startX = 0.05f);

    void draw(GLuint shader);
    void moveLeft(float liftX0, bool doorsOpen);
    void moveRight(float liftX0,float lixtX1, bool doorsOpen, int liftFloor);
    float getPosX() { return posX; }
    int getFloor() { return floor; }
    bool touchesLift(float liftX0);
    void syncWithLift(float liftY0, int liftFloor);
    float getRightX() { return x0 + posX + width; }


};
