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

public:
    Person(int startFloor, float floorSpacing, float width = 0.1f, float heightFactor = 0.7f, float startX = 0.05f);

    void draw(GLuint shader);
    void moveLeft();
    void moveRight(float liftX0);
    float getPosX() { return posX; }
    int getFloor() { return floor; }
    bool touchesLift(float liftX0);
    float getRightX() { return x0 + posX + width; }


};
