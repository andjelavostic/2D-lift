#pragma once
#include <GL/glew.h>
#include <vector>
using namespace std;

class PanelGrid {
private:
    int rows, cols;
    vector<GLuint> VAOs;
    vector<GLuint> VBOs;
    float color[3];

public:
    // Konstruktor
    PanelGrid(float left, float right, float bottom, float top,
        int r, int c, float buttonWidth, float buttonHeight,
        float hSpacing, float vSpacing,
        float red = 0.6f, float green = 0.6f, float blue = 0.6f);

    // Crtanje svih dugmica
    void draw(GLuint shader);

    // Destruktor
    ~PanelGrid();
};

