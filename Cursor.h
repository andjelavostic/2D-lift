#pragma once
#include <GL/glew.h>

class Cursor {
public:
    Cursor();
    ~Cursor();

    // postavljanje ventilacije (boja)
    void setVentilation(bool on) { ventilationOn = on; }

    // crtanje kursora
    void draw(GLuint shader, double mouseX, double mouseY, int screenWidth, int screenHeight);

    // inicijalizacija tekstura
    void loadTextures(const char* blackPath, const char* colorPath);

private:
    GLuint cursorVAO, cursorVBO;
    GLuint cursorBlackTex, cursorColorTex;
    bool ventilationOn = false;
};
