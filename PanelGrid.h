#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
using namespace std;

class PanelGrid {
private:
    int rows, cols;
    float color[3];
    struct Button {
        GLuint VAO;
        GLuint VBO;
        GLuint texture;
    };
    std::vector<Button> buttons;
    std::vector<std::string> texturePaths = {
    "resources/su.png",
    "resources/pr.png",
    "resources/number-one.png",
    "resources/number-2.png",
    "resources/number-3.png",
    "resources/number-four.png",
    "resources/number-five.png",
    "resources/number-six.png",
    "resources/close.png",
    "resources/open.png",
    "resources/stop.png",
    "resources/fan.png"
    };
    struct FloorButton {
        int floor;         // broj sprata
        bool pressed;      // da li je taster trenutno pritisnut
        bool highlight;    // da li da se crta uokviren
    };
    std::vector<FloorButton> floorButtons;


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

