#pragma once
#include <GL/glew.h>

class Panel {
private:
    GLuint VAO, VBO;
    float vertices[8]; // 4 temena * 2 koordinate
    float color[3];

public:
    // Konstruktor
    Panel(float x0, float x1, float y0, float y1, float r = 0.6f, float g = 0.6f, float b = 0.6f);

    // Crtanje panela
    void draw(GLuint shader);

    // Destruktor
    ~Panel();
};
