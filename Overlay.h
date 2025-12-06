#pragma once
#include <string>
#include <GL/glew.h>
#include "../2D-lift/Header/Util.h"
class Overlay {
public:
    Overlay();
    ~Overlay();

    // pozicija u NDC [-1,1], npr. (-0.9, 0.9) gornji levi ugao
    void setPosition(float x, float y);

    // veli?ina overlay kvadrata (width, height u NDC)
    void setSize(float width, float height);

    // crtanje overlay-a
    void draw(GLuint shader);

private:
    GLuint textureID;
    GLuint vao, vbo;
    float posX, posY;
    float width, height;
};
